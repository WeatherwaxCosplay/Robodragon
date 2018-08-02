/*******************************************************************************************************************************

  Robodragon 5

  WeatherWax Cosplay / SEMU Consulting - 2018

  Control program for robotic dragon head, incorporating
  - pan and tilt server for 'head', congrolled via left gamepad joystick
  - 2 x SSD1331 OLED displays for 'eyes', controlled via right gamepad joystick

  The image files used for the eyes were created using Vladimir Riuson's excellent lcd-image-convertor utility
  (remember to amend the default image.tmpl template to add a PROGMEM directive at the top of the data array).
  (https://github.com/riuson/lcd-image-converter)

  WIRING CONNECTIONS:

  XBee:

  TX/DOUT on XBee   --> RX1 on Arduino
  RX/DIN on XBee    --> TX1 on Arduino

  SSD1331 OLEDs:

  NB: Multiple OLED boards should share the same SPI MISO, SCLK, RESET and DC lines but each board must have a separate SPI CS (OC) line.

  GND on both OLED boards (brown wires)     --> GND on Arduino
  VCC on both OLED boards (red wires)       --> 5V on Arduino
  SCL on both OLED boards (orange wires)    --> Digital pin 13 (SPI hardware SCLK) on Arduino
  SDA on both OLED boards (yellow wires)    --> Digital pin 11 (SPI MOSI) on Arduino - this is the chip select pin for the OLED itself
  RES on both OLED boards (green wires)     --> Digital pin 10 (SPI RESET) on Arduino
  DC on both OLED boards (blue wires)       --> Digital pin 9 (SPI Data Channel) on Arduino
  CS on left OLED board (purple wire)       --> Digital pin 8 (SPI Chip Select) on Arduino
  CS on right OLED board (purple/grey wire) --> Digital pin 7 (SPI Chip Select) on Arduino

  Adafruit SoundFX Board:

  SoundBoard  Arduino
  -------------------
  VIN         5V
  GND         GND
  UG          GND
  RST         Pin 4
  RX          TX2 (Pin 16)
  TX          RX2 (Pin 17)
  L           +ve speaker connection
  GND         -ve speaker connection
******************************************************************************************************************************/

//#define DEBUG 1

#include <Adafruit_GFX.h>
#include <SSD1331_SPI_CONTROL.h>
#include <SPI.h>
#include <Servo.h>
#include <Gamepad_XBee.h>
#include <Adafruit_Soundboard.h>

#include "_images/deye.h"
#include "_images/maskl1.h"
#include "_images/maskl2.h"
#include "_images/maskr1.h"
#include "_images/maskr2.h"
#include "_images/mask3.h"
#include "_images/mask4.h"

#define TX_ADDRESS  0x0000 // set to the gamepad XBee's MY address
#define sclk 13   // marked SCL on board
#define mosi 11   // marked SDA on board
#define rst  10   // marked RES on board
#define rst2 99   // not connected if using common reset pin 
#define dc   9    // marked DC on board
#define cs   8    // marked CS on left board
#define cs2  7    // marked CS on right board
#define LEFT 0
#define RIGHT 1
#define SFX_RST 4 // RST pin on the sound board

const tImage maskL[4] = {
  maskl1, maskl2, mask3, mask4
};
const tImage maskR[4] = {
  maskr1, maskr2, mask3, mask4
};

// set up array of SPI displays - uses hardware SPI pins on Teensy (MOSI 11 SCLK 13)
SSD1331_SPI_CONTROL displays[] = {
  SSD1331_SPI_CONTROL(cs, dc, rst),
  SSD1331_SPI_CONTROL(cs2, dc, rst2)
};

Gamepad_XBee gamepad = Gamepad_XBee();
//Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial2, NULL, SFX_RST);

Servo xservo;
Servo yservo;
Servo lservo;
Servo rservo;
uint16_t lastx;
uint16_t lasty;
uint16_t wings = 70;
uint16_t min_delta = 5;
uint16_t max_delta = 1023;
uint32_t blinkPeriod = 4000;
uint32_t t0 = 0;
uint8_t blinkMaskL = 0;
uint8_t blinkMaskR = 0;
uint8_t blinkIter = 0;
char cmd;
uint16_t flapd = 15;
bool autoMode = false;
uint32_t autoPeriod;
bool inMotion = false;
uint16_t eyex = 512;
uint16_t eyey = 512;
uint16_t headx = 512;
uint16_t heady = 512;
uint16_t targetx;
uint16_t targety;
bool autoFlap = false;

//**************************************************************************
// Setup routine
//**************************************************************************
void setup(void) {

  displays[LEFT].begin();
  displays[RIGHT].begin();

  Serial1.begin(57600); // XBee
  //Serial2.begin(9600);  // SoundFX Board
  gamepad.beginRX(Serial1, TX_ADDRESS);

  xservo.attach(2);
  yservo.attach(3);
  lservo.attach(4);
  rservo.attach(5);

  randomSeed(analogRead(0));

}

//**************************************************************************
// Main loop. Polls gamepad inputs and sets displays and servos accordingly
//**************************************************************************
void loop() {

  if (gamepad.receiveData() == RX_OK) {

    t0 = millis(); // remember loop start time

    blinkTimer(); // refresh blink timer

    // Pushing L1 and Start button simultaneously puts dragon into autonomous mode
    // Pushing L1 and Select button simultaneously puts dragon into manual (remote control) mode
    if (gamepad.Button_L1 && gamepad.Button_Select && autoMode) {
      autoMode = false;
      displayMsg("AUTO OFF", RED);
    }
    if (gamepad.Button_L1 && gamepad.Button_Start && !autoMode) {
      autoMode = true;
      displayMsg("AUTO ON", GREEN);
    }

    if (autoMode) {

      autoTimer(); // refresh auto timer

      setEyes (eyex, eyey);
      setHead (headx, heady);
      if (autoFlap) {
        setWings ();
      }

    }
    else {

      setEyes (gamepad.RightJoystick_X, gamepad.RightJoystick_Y);
      setHead (gamepad.LeftJoystick_X, gamepad.LeftJoystick_Y);
      if (gamepad.Button_R2) {
        setWings ();
      }
      //setSound ();

      while (t0 > millis() - POLL_INTERVAL) {} // wait until polling interval expires

    }

  }

}

//**************************************************************************
// Sets position of left and right eyes, with eyelid mask overlay governed
// by random blink interval
//**************************************************************************
void setEyes(uint16_t x, uint16_t y) {

  if (x <= max_delta && y <= max_delta) {

    x = map(x, 0, 1023, TFTWIDTH - 1, 0);
    y = map(y, 0, 1023, TFTHEIGHT - 1, 0);

    displays[LEFT].drawMaskedSegment(x, y, &deye, &maskL[blinkMaskL]);
    displays[RIGHT].drawMaskedSegment(x, y, &deye, &maskR[blinkMaskR]);

  }

}

//**************************************************************************
// Sets position of head pan (x) and tilt (y) servos
//**************************************************************************
void setHead (uint16_t x, uint16_t y) {

  if (x <= max_delta && y <= max_delta) {

    if (abs(lastx - x) > min_delta) {
      lastx = x;
      x = map(x, 0, 1023, 160, 20);
      xservo.write(x);
    }

    if (abs(lasty - y) > min_delta) {
      lasty = y;
      y = map(y, 0, 1023, 25, 100);
      yservo.write(y);
    }

  }

}

//**************************************************************************
// Flaps left and right wings up and down
//**************************************************************************
void setWings () {

  if (wings >= 90) {
    flapd = -15;
  }
  else if (wings <= 50) {
    flapd = 15;
  }
  wings += flapd;

  lservo.write(wings);
  rservo.write(180 - wings);

}

/***
  void setSound() {

  flushInput();
  cmd = ' ';

  if (gamepad.Button_1) {
    cmd = '1';
  }
  else if (gamepad.Button_2) {
    cmd = '2';
  }
  else if (gamepad.Button_3) {
    cmd = '3';
  }
  else if (gamepad.Button_4) {
    cmd = '4';
  }
  else if (gamepad.Button_R2) {
    cmd = 'q';
  }

  switch (cmd) {
    case '1': {
        sfx.playTrack((uint8_t)0);
        break;
      }
    case '2': {
        sfx.playTrack((uint8_t)1);
        break;
      }
    case '3': {
        sfx.playTrack((uint8_t)2);
        break;
      }
    case '4': {
        sfx.playTrack((uint8_t)3);
        break;
      }
    case 'q': {
        sfx.stop();
        break;
      }
    default:
      break;
  }

  }

  void flushInput() {
  // Read all available serial input to flush pending data.
  uint16_t timeoutloop = 0;
  while (timeoutloop++ < 40) {
    while (Serial2.available()) {
      Serial2.read();
      timeoutloop = 0;  // If char was received reset the timer
    }
    delay(1);
  }
  }
***/

//**************************************************************************
// Refreshes blink interval timer and eyelid display mask
// Blink interval is random between 1 and 3 seconds
//**************************************************************************
void blinkTimer () {

  // right joystick button press causes eyes to immediately glare.
  if (gamepad.RightJoystick_B) {
    blinkMaskL = 2;
    blinkMaskR = 2;
    blinkIter = 4;
    blinkPeriod = millis() + 30;
  }
  // otherwise blink periodically
  else if (millis() > blinkPeriod) {

    switch (blinkIter) {
      case 0:
        blinkPeriod = millis() + random(800, 3000);
        blinkMaskL = 0;
        blinkMaskR = 0;
        break;
      case 1:
        blinkPeriod = millis() + 25;
        blinkMaskL = 1;
        blinkMaskR = 1;
        break;
      case 2:
        blinkPeriod = millis() + 25;
        blinkMaskL = 2;
        blinkMaskR = 2;
        break;
      case 3:
        blinkPeriod = millis() + 25;
        blinkMaskL = 3;
        blinkMaskR = 3;
        break;
      case 4:
        blinkPeriod = millis() + 30;
        blinkMaskL = 2;
        blinkMaskR = 2;
        break;
      case 5:
        blinkPeriod = millis() + 30;
        blinkMaskL = 1;
        blinkMaskR = 1;
        break;
    }

    blinkIter++;
    if (blinkIter > 5) {
      blinkIter = 0;
    }

  }

}

//**************************************************************************
// Generates random x,y target at periodic intervals and then increments head
// and eye positions towards that target.
//**************************************************************************
void autoTimer () {

  int16_t inc = 0;
  uint16_t s = 5;

  if (millis() > autoPeriod) {

    switch (inMotion) {
      case false:
        autoPeriod = millis() + random(2000, 5000);
        inMotion = true;
        targetx = random(200, 823);
        targety = random(200, 823);
        autoFlap = false;
        if (random(1, 5) == 1) {
          autoFlap = true;
        }
        break;
      case true:
        autoPeriod = millis() + 10;

        inc = targetx - headx;
        headx += inc / s;
        inc = targety - heady;
        heady += inc / s;

        inc = targetx - eyex;
        eyex += inc / s;
        inc = targety - eyey;
        eyey += inc / s;

        if (abs(targetx - headx) <= s && abs(targety - heady) <= s) {
          inMotion = false;
        }
        break;
    }

  }

}

void displayMsg(String s, uint16_t c) {

  for (uint8_t i = 0; i < 2; i++) {
    displays[i].fillScreen(BLACK);
    displays[i].setCursor(0, 30);
    displays[i].setTextColor(c);
    displays[i].setTextSize(2);
    displays[i].println(s);
    //displays[i].fillScreen(BLACK);
  }
}


