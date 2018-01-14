/***************************************************
  Test harness for controlling multiple servos via an I2C PWM servo board
  and a PS4 Dualshock bluetooth I2C USB host controller board

****************************************************/

#define DEBUG 0

#include <Adafruit_GFX.h>
#include <SPI.h>
#include <SSD1331_SPI_CONTROL.h>
#include <SERVO_I2C_CONTROL.h>
#include <DS4_I2C_CONTROL.h>

#include "_images/eye01.h"
#include "_images/eye02.h"
#include "_images/eye03.h"
#include "_images/eye04.h"
#include "_images/eye05.h"
#include "_images/eye06.h"
#include "_images/eye07.h"
#include "_images/eye08.h"
#include "_images/eye09.h"
#include "_images/eye10.h"
#include "_images/eye11.h"
#include "_images/eye12.h"
#include "_images/eye13.h"
#include "_images/eye14.h"
#include "_images/eye15.h"

#define sclk          13  // SPI clock - marked 'CK' on most SSD1331 OLED boards
#define mosi          11  // SPI master out, slave in - marked 'SI' on most SSD1331 OLED boards
#define cs            10  // SPI chip select - marked 'OC' on most SSD1331 OLED boards. NB: The 'SC' pin is a separate chip select for the SD card, if fitted.
#define rst           9   // SPI reset - marked 'R' on most SSD1331 OLED boards
#define dc            8   // SPI data - marked 'DC' on most SSD1331 OLED boards
#define cs2           7   // chip select for a second OLED board
#define rst2          99  // fictional reset pin for a second OLED board (the SPI reset routine is common to all devices on the bus)

#define EYEMATRIX_X   5   // number of eye images in x axis
#define EYEMATRIX_Y   3   // numer of eye images in y axis
#define FLAPOFF       0
#define FLAPUP        1
#define FLAPDOWN      2

// set up I2C servo and DS4 controller boards
SERVO_I2C_CONTROL servos = SERVO_I2C_CONTROL(0x40);
DS4_I2C_CONTROL ds4 = DS4_I2C_CONTROL(0x29);

// set up array of eyeball images
const tImage eyes[15] = {
  eye01, eye02, eye03, eye04, eye05, eye06, eye07,
  eye08, eye09, eye10, eye11, eye12, eye13, eye14, eye15
};

// set up array of SPI displays - uses hardware SPI pins on Teensy (MOSI 11 SCLK 13)
SSD1331_SPI_CONTROL displays[] = {
  SSD1331_SPI_CONTROL(cs, dc, rst) ,
  SSD1331_SPI_CONTROL(cs2, dc, rst2)
};

uint8_t servo_pan = 0;
uint8_t servo_tilt = 2;
uint8_t servo_wing_left = 4;
uint8_t servo_wing_right = 6;
uint8_t servo_type = 3; // HS-85BB pulse range 0.553ms to 2.3ms for 180 degree travel
float freq = 60;     // standard 60Hz
uint32_t baudrate = 115200;
uint16_t timer;
uint8_t read_interval = 20;
uint8_t pan = 0;
uint8_t tilt = 0;
uint8_t flap = 0;
uint8_t eyex = 0;
uint8_t eyey = 0;
uint8_t lastpan = 0;
uint8_t flapstate = FLAPUP;
uint8_t lasttilt = 0;
uint16_t lasteyex = 0;
uint16_t lasteyey = 0;
uint16_t lastflap = 0;
uint8_t min_delta = 5;

//*************************************************************
// SETUP
//*************************************************************
void setup() {
  if (DEBUG) {
    while (!Serial);
    Serial.begin(baudrate);
  }
  timer = 0;

  servos.begin();
  servos.setPWMFreq(freq);

  if (DEBUG) {
    Serial.println((String) "Clearing displays");
  }

  for (uint8_t i = 0; i < 2; i++) {
    displays[i].begin();
    displays[i].clearWindow();
  }

}

//*************************************************************
// MAIN LOOP
//*************************************************************
void loop() {
  if (millis() > timer)
  {
    timer = millis() + read_interval;
    ds4.get_ps4();
    if (ds4.ps4_ok)
    {
      setHead();
      setEyes();
      setWings();
    }
  }

}

//*************************************************************
// Control pan and tilt of head
//*************************************************************
void setHead() {

  // only reset servo if ps4 left joystick value has changed significantly since last read

  pan = ds4.l_joystick_x;
  if (DEBUG) {
    Serial.println((String) "Pan = " + pan);
  }
  if (abs(lastpan - pan) > min_delta) {
    lastpan = pan;
    servos.setDegrees(servo_pan, servo_type, map(pan, 0, 255, 180, 0));
  }
  tilt = ds4.l_joystick_y;
  if (DEBUG) {
    Serial.println((String) "Tilt = " + tilt);
  }
  if (abs(lasttilt - tilt) > min_delta ) {
    lasttilt = tilt;
    servos.setDegrees(servo_tilt, servo_type, map(tilt, 0, 255, 180, 0));
  }

}

//*************************************************************
// Control wing flap state
//*************************************************************
void setWings() {

  // only reset servo if ps4 left joystick value has changed significantly since last read

  flap = ds4.tpad_x;
  if (DEBUG) {
    Serial.println((String) "Flap = " + flap);
  }
  if (abs(lastflap - flap) > min_delta) {
    lastflap = flap;
    servos.setDegrees(servo_wing_left, 0, map(flap, 255, 0, 180, 0));
    servos.setDegrees(servo_wing_right, 0, map(flap, 255, 0, 0, 180));
  }

}

//*************************************************************
// Control eye and blink motion
//*************************************************************
void setEyes() {

  // only reset eyes if ps4 right joystick value has changed significantly since last read

  uint8_t i, idx;

  eyex = map(ds4.r_joystick_x, 0, 255, 12, TFTWIDTH - 1);
  eyey = map(ds4.r_joystick_y, 0, 255, 12, TFTHEIGHT - 1);
  if (DEBUG) {
    Serial.println((String) "Coordinates = " + eyex + " , " + eyey);
  }

  if (abs(lasteyey - eyey) > min_delta || abs(lasteyex - eyex) > min_delta ) {
    lasteyex = eyex;
    lasteyey = eyey;

    // get the index of the image corresponding to these coordinates
    idx = getImageIndex(eyex, eyey);

    // display the image on each OLED
    for (i = 0; i < 2; i++) {
      //displays[i].drawImage(&eyes[idx]);
      displays[i].clearWindow();
      displays[i].fillCircle(eyex, eyey, 12, GREEN);
      displays[i].fillCircle(eyex, eyey, 6, BLACK);
    }
  }

}

//*************************************************************
// gets index of dragon eye image based on controller x,y coordinates
// this is based on a 5x3 matrix with 15 images in total
//*************************************************************
uint8_t getImageIndex(uint16_t x, uint16_t y) {

  uint8_t i;

  if (x >= TFTWIDTH) {
    x = TFTWIDTH - 1;
  }
  if (y >= TFTHEIGHT) {
    y = TFTHEIGHT - 1;
  }

  i = floor(((float)x * EYEMATRIX_X / TFTWIDTH)) + (floor(((float)y * EYEMATRIX_Y / TFTHEIGHT)) * EYEMATRIX_X);
  if (DEBUG) {
    Serial.println((String) "Image index = " + i);
  }
  return i;
}

