# robodragon
"Animatronix" cosplay dragon project

This originated as a cosplay project to create a life-size (i.e. about the size of a cat) 'animatronix' dragon based on the Terry Pratchett character 
Vinny aka Lord Mountjoy Quickfang Winterforth IV, who appears in the Discworld volume Guards! Guards!

![discworld dragon](/discworlddragon.png)

The project provisionally incorporates the following features, though not all may make it to the final version:

1. panning and tilting head via standard RC servos.
2. flapping wings via standard RC servos.
3. articulated mouth and/or tail via standard RC servos.
3. animated 'moving' and 'blinking' eyes via a pair of 0.95" SPI OLED displays and a sequence of bitmap images.
4. smoke generator utilising repurposed aquarium pump motor and electronic vape stick (subject to cosplay event regulations).
5. 'roaring' and other sounds via internal speaker and UART audio fx board.
6. various illumination via RGB LEDs.
7. all the above remotely controllable via XBee-wireless equipped gamepad-style controller, in addition to a pre-programmed autonomous mode.

![provisional wiring diagram](/wiringdiagram.png)

## Work in Progress Pictures

![robodragon head wiring matrix 1](/dragonheadwiringmatrix1.jpg)

![robodragon head wiring matrix 2](/dragonheadwiringmatrix2.jpg)

![robodragon head wiring matrix 3](/dragonheadwiringmatrix3.jpg)

## Current Status

1. Based on Teensy 3.2 or 3.6 MCU, mainly due to processor speed and program memory demands. Also works on Arduino Due but will NOT work on standard Arduino UNO.
2. Core programming complete for both remote control and stand-alone (pre-programmed) modes.
3. Arduino code libraries developed for the wireless gamepad controller and SPI displays. These can be found at
[SEMU_SSD1331](https://github.com/semuconsulting/SEMU_SSD1331.git) and [Gamepad_XBee](https://github.com/semuconsulting/Gamepad_DS4_I2C.git).
4. Mechanical framework for head and wings complete and tested.
5. Remaining task is to incorporate electronics and mechanics into dragon 'body'.
