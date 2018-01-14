# robodragon
"Animatronix" cosplay dragon project

This originated as a cosplay project to create a life-size (i.e. about the size of a cat) 'animatronix' dragon based on the Terry Pratchett Errol character http://wiki.lspace.org/mediawiki/Errol.

The project provisionally incorporates the following features, though not all may make it to the final version:

1. panning and tilting head via standard RC servos.
2. flapping wings via standard RC servos.
3. articulated mouth and/or tail via standard RC servos.
3. animated 'moving' and 'blinking eyes' via a pair of 0.95" SPI OLED displays and a sequence of bitmaps images.
4. smoke generator utilising repurposed aquarium pump motor and electronic vape stick (subject to cosplay event regulations).
5. 'roaring' and other sounds via internal speaker and audio samples.
6. various illumination via RGB LEDs.
7. all the above remotely controllable via XBee-wireless equipped gamepad-style controller, in addition to a pre-programmed autonomous mode.

## Current Status

1. After some deliberation, the project has settled on Arduino-compatible MCUs in preference to an earlier Raspberry Pi 3 design. The Teensy 3.2 has been selected on the basis of its small form factor, high processor speed and large flash and program memory capacity.
2. Currently still at the design and prototype stage.
3. Arduino code libraries developed for the wireless gamepad controller and SPI displays.
4. Principal issues have been a) working within Teensy 3.2 memory constraints, and b) been getting the SPI displays to be stable in a mobile cosplay context.
