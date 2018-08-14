/*******************************************************************************************************************************

  Robodragon

  WeatherWax Cosplay / SEMU Consulting - 2018

  Control program for robotic dragon head, incorporating
  - pan and tilt servos for 'head', controlled via left gamepad joystick
  - 2 x SSD1331 OLED displays for 'eyes', controlled via right gamepad joystick
  - various actions can be programmed using other gamepad controls

  NB: if you're planning to use a UART (Serial) XBee or USB Host board together with
  a UART Sound FX board, you'll need to ensure your MCU has at least two hardware serial ports.
  Using SoftwareSerial to get round hardware serial port constraints won't necessarily work here
  and may cause other library incompatibility issues.

  The image files used for the eyes were created using Vladimir Riuson's excellent lcd-image-convertor utility
  (remember to amend the default image.tmpl template to add a PROGMEM directive at the top of the data array).
  (https://github.com/riuson/lcd-image-converter)

  WIRING CONNECTIONS:

  Receiving XBee or USB Host Board in Serial (rather than I2C) mode:
  (NB: we're using Serial1 rather than Serial or SerialUSB here)

  XBee or USB Host Board            Slave Arduino/Teensy
  ----------------------------------------------------------------------------------
  TX/DOUT on XBee or USB Host       RX1 (pin 19 on the Due (NOT 0), pin 0 on the Teensy 3.x)
  RX/DIN on XBee or USB Host        TX1 (pin 18 on the Due (NOT 1), pin 1 on the Teensy 3.x)

  SSD1331 OLEDs:

  NB: Multiple OLED boards should share the same SPI MISO, SCLK, RESET and DC lines but each board must have a separate SPI CS (OC) line.

  OLED display                                Slave Arduino/Teensy
  -------------------------------------------------------------------------------
  GND on both OLED boards (brown wires)       GND
  VCC on both OLED boards (red wires)         5V
  SCL on both OLED boards (orange wires)      Digital pin 13 (SPI hardware SCLK)
  SDA on both OLED boards (yellow wires)      Digital pin 11 (SPI hardware MOSI)
  RES on both OLED boards (green wires)       Digital pin 10 (SPI RESET)
  DC on both OLED boards (blue wires)         Digital pin 9 (SPI Data Channel)
  CS on left OLED board (purple wire)         Digital pin 8 (SPI Chip Select)
  CS on right OLED board (purple/grey wire)   Digital pin 7 (SPI Chip Select)

  Adafruit SoundFX Board:
  (NB: we're using Serial2 here - make sure your MCU has at least 2 hardware UART ports)

  SoundBoard    Slave Arduino/Teensy
  -----------------------------------
  VIN           5V
  GND           GND
  UG            GND
  RST           Digital Pin 4
  RX            TX2 (pin 16 on Due, pin 10 on Teensy 3.x)
  TX            RX2 (pin 17 on Due, pin 9 on Teensy 3.x)
  L             +ve speaker connection
  GND           -ve speaker connection
******************************************************************************************************************************/

//#define GAMEPAD_DFR     // use only this for DFRobot controller with XBee transmitter
//#define GAMEPAD_PS3   // use only this for PS3 controller in USB mode with XBee transmitter
#define GAMEPAD_PS4BT // use only this for PS3/4 controller in Bluetooth mode

#include <SEMU_SSD1331.h>
#include <Servo.h>
#include <Adafruit_Soundboard.h>
#include <colors.h>

#if defined GAMEPAD_DFR
#include <gamepad_dfr.h>
#elif defined GAMEPAD_PS3
#include <gamepad_ps3.h>
#elif defined GAMEPAD_PS4BT
#include <gamepad_ps4bt.h>
#endif

#include "_images/deye.h"
#include "_images/maskl1.h"
#include "_images/maskl2.h"
#include "_images/maskr1.h"
#include "_images/maskr2.h"
#include "_images/mask3.h"
#include "_images/mask4.h"

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
SEMU_SSD1331 displays[] = {
  SEMU_SSD1331(cs, dc, rst),
  //SEMU_SSD1331(cs2, dc, rst2)
};

#if defined GAMEPAD_DFR
Gamepad_Dfr gamepad = Gamepad_Dfr();
#elif defined GAMEPAD_PS3
Gamepad_PS3 gamepad = Gamepad_PS3();
#elif defined GAMEPAD_PS4BT
Gamepad_PS4BT gamepad = Gamepad_PS4BT();
#endif

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
  //displays[RIGHT].begin();

  Serial1.begin(57600); // XBee
  //Serial2.begin(9600);  // SoundFX Board
  gamepad.beginRX(Serial1);

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

  t0 = millis(); // remember loop start time
  
  gamepad.get_data();

  blinkTimer(); // refresh blink timer

  // Pushing L1 and UP button simultaneously puts dragon into autonomous mode
  // Pushing L1 and DOWN button simultaneously puts dragon into manual (remote control) mode
  if (gamepad.button_l1 && gamepad.button_up && autoMode) {
    autoMode = false;
    displayMsg("AUTO OFF", RED);
  }
  if (gamepad.button_l1 && gamepad.button_down && !autoMode) {
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

    setEyes (gamepad.r_joystick_x, gamepad.r_joystick_y);
    setHead (gamepad.l_joystick_x, gamepad.l_joystick_y);
    if (gamepad.button_r2) {
      setWings ();
    }
    //setSound ();

    while (t0 > millis() - POLL_INTERVAL) {} // wait until polling interval expires

  }

}

//**************************************************************************
// Sets position of left and right eyes, with eyelid mask overlay governed
// by random blink interval
//**************************************************************************
void setEyes(uint16_t x, uint16_t y) {

  if (x <= max_delta && y <= max_delta) {

    x = map(x, 0, CTL_MAX, displays[LEFT].TFTWIDTH - 1, 0);
    y = map(y, 0, CTL_MAX, displays[LEFT].TFTHEIGHT - 1, 0);

    displays[LEFT].drawMaskedSegment(x, y, &deye, &maskL[blinkMaskL]);
    //displays[RIGHT].drawMaskedSegment(x, y, &deye, &maskR[blinkMaskR]);

  }

}

//**************************************************************************
// Sets position of head pan (x) and tilt (y) servos
//**************************************************************************
void setHead (uint16_t x, uint16_t y) {

  if (x <= max_delta && y <= max_delta) {

    if (abs(lastx - x) > min_delta) {
      lastx = x;
      x = map(x, 0, CTL_MAX, 160, 20);
      xservo.write(x);
    }

    if (abs(lasty - y) > min_delta) {
      lasty = y;
      y = map(y, 0, CTL_MAX, 25, 100);
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

  // pressing LEFT button press causes eyes to immediately 'glare'
  if (gamepad.button_left) {
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


