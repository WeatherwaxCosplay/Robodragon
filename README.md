# Robodragon
"Animatronix" cosplay dragon project

This originated as a cosplay project to create a life-size (i.e. about the size of a cat) 'animatronix' dragon based on the Terry Pratchett character 
Vinny aka Lord Mountjoy Quickfang Winterforth IV, who appears in the Discworld volume Guards! Guards!

![discworld dragon](/docs/discworlddragon.png)

## Provisional functionality and wiring diagram

The project provisionally incorporates the following features, though not all may make it to the final version:

1. Panning and tilting head via standard RC servos.
2. Flapping wings via standard RC servos.
3. Articulated mouth and/or tail via standard RC servos.
4. Animated 'moving' and 'blinking' eyes via a pair of 0.95" SPI OLED displays and a sequence of masked bitmaps.
5. 'Roaring' and other sounds via an internal miniature powered speaker and UART audio fx board.
6. Various illumination via RGB LEDs.
7. Smoke generator utilising repurposed aquarium pump motor and electronic vape stick filled with diluted medicinal glycerine (subject to cosplay event regulations - 
smoke effects normally prohibited in the general auditorium).
8. All the above remotely controllable via a gamepad-style controller, in addition to a pre-programmed autonomous mode.

![provisional wiring diagram](/docs/wiringdiagram.png)

## Current Status

1. Currently favouring the 32-bit Teensy 3.x microcontroller (MCU), mainly due to small form factor and excellent processor speed and program memory capacity. 
One downside to the Teensy platform is that the header pins have to be soldered by the end user, which is probably not a task for a soldering novice. 
If the soldering is daunting, the code also works on most 32-bit Arduino platforms such as the Due or Zero, but a standard 8-bit Arduino UNO doesn't have have enough grunt or memory to 
support all planned functionality. There are minor differences between the Teensy software libraries and some of the standard Arduino (Adafruit) libraries, but I have not found these to 
be a problem in practice.
2. Key challenge has been getting the OLED displays to work reliably as part of a moving assembly. The SPI protocols used by these displays are only intended for transmission 
over very short and rigid tracks (typically a few cm at most), so this project is approaching the limit of what is practically feasible using SPI devices. Much depends on the
quality of the soldering (or crimping) and cabling. [Electro-mechanical animatronic eyes](https://www.pinterest.co.uk/pin/562035228467357323/) were considered, but these would have made 
the head unfeasibly large and you lose the ability to alter the look and feel programatically (which is half the fun).
3. Arduino code libraries developed for the SPI displays and a variety of wireless gamepad controllers. These can be found at https://github.com/semuconsulting/
4. Mechanical jig for head and wings complete and tested.
5. Remaining task is to incorporate the electro-mechanics into a convincing dragon 'body' and make the whole thing robust and reliable enough for the rigours of cosplay.

## Work in Progress Pictures

Head wiring matrix rear                               | Head wiring matrix front                           
:-------------------------------------------------:   | :-----------------------:
![wiring matrix 2](/docs/dragonheadwiringmatrix2.jpg) | ![wiring matrix 3](/docs/dragonheadwiringmatrix3.jpg)
**Teensy robotics shield \***                         | **Teensy robotics shield**
![Teensy shield 1](/docs/Teensyshield1.jpg)           | ![Teensy shield 2](/docs/Teensyshield2.jpg)
**Smoke generator rig**                               | **Vape stick adapter**
![Smoke Generator Rig](/docs/smokegenrig.png)         | ![Vape stick adapter](/docs/vapeadapter.png) 

\* The compact [Hobbytronics Teensy robotics shield](http://hobbytronics.co.uk/teensy-boards/teensy-robot-controller) originally adapted for this prototype (by repurposing some of the PWM servo headers as an SPI interface) is sadly no longer available. There are other off-the-shelf alternatives, such as this [DFRobot IO expansion shield](https://www.dfrobot.com/product-1009.html) (which can be used with an Arduino Due), but they're generally tied to a standard Arduino UNO form factor and pinout.

## Wireless Remote Controller Options

See the included RC_OPTIONS.MD file for an exploration of various wireless controller options.
