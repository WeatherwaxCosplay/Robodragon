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
over very short and rigid tracks (typically a few cm at most), so this project is approaching the limit of what is practically feasible using SPI devices. [Electro-mechanical 
animatronic eyes](https://www.pinterest.co.uk/pin/562035228467357323/) were considered, but these would have made the head unfeasibly large and you lose the ability to 
alter the look and feel programatically.
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

## Gamepad Remote Control Options

A variety of gamepad-style wireless remote control configurations are available, some of which are illustrated below. I have experimented with most of these at various point and 
have endeavoured to develop (or adapt from existing sources) a [simple, standardised set of Arduino gamepad software libraries](https://github.com/semuconsulting) to 
allow end users to adopt different configurations more or less interchangeably. See the README files in these libraries for further details.

PS3 USB/XBee --> XBee   | PS3/4 --> Bluetooth    | DFRobot XBee --> XBee
----------------------- | :--------------------: | :--------------------:
![ps3](/docs/Configurationps3.png) | ![ps4bt](/docs/Configurationps4bt.png) | ![dfr](/docs/Configurationdfr.png)

### Standard PS3/PS4 Controller
Perhaps the most reliable and intuitive option is to use a standard PS3 or PS4 controller (or the XBox equivalent) in conjunction with some form of wireless transceiver - either Bluetooth or 
a longer range RF system like [XBee](https://en.wikipedia.org/wiki/XBee). Using Bluetooth reduces the overall parts count and cost, as both PS3 and PS4 
controllers already incorporate Bluetooth transmitters. The key disadvantages of Bluetooth are a) limited range, and b) the need to 'pair' the controller 
with the Bluetooth receiver and retain this pairing after powering down, which can be problematic in an Arduino hardware context. 

If Bluetooth proves unreliable, it is also possible to use a PS3 controller connected via a physical USB cable to a [portable USB Host adapter and XBee transmitter](http://www.hobbytronics.co.uk/usb-host-xbee-shield), 
though this does increase the parts count and cost and is a little more cumbersome to carry around. I've personally found this to be a more reliable option in the past, though 
Bluetooth support is improving in the Arduino hobbyist community.

Another disadvantage with the PS3/PS4 option is that these are proprietary platforms and you are generally reliant on closed-source, reverse-engineered 
USB software libraries to interpret the PS3/PS4 output. 
There is an active [open-source project](https://github.com/felis/UHS30) working on opening up such USB protocols, and also allowing the use of native USB ports provided by some 
of the more capable Arduino platforms (such as the Due or Teensy) - obviating the need to use an external USB Host adapter or shield - but at time of writing this was still at 
an early Alpha stage.

There are third-party PS3/PS4 clones but the build quality often leaves a lot to be desired. Genuine second-hand PS3 controllers in good condition can 
be found on eBay for around £15 and I'd recommend sticking with the genuine article.

### Third-party or DIY Arduino-based Gamepad Controller
An alternative option is something like the [DFRobot PS3-style controller](https://www.dfrobot.com/product-858.html) which embeds a programmable Arduino Leonardo-compatible MCU with 
a slot for an XBee radio module (either RF or Bluetooth). This has the advantage that you can program the transmitting MCU yourself and therefore have complete control over its 
functionality, and you can select different XBee platforms depending on your wireless needs.

The disadvantages of the DFRobot controller option are a) it uses hobby-grade components and the build quality and control responsiveness are nothing like as good as a genuine 
PS3/PS4 controller, b) it's not particularly cheap (around £50 in the UK, but may need to be delivered from China, incurring additional import charges) and c) the documentation is poor.

You could, at a push, build your own equivalent wireless 'gamepad' using components available from most electronic hobby stores:
 
* a couple of [thumb joysticks](http://www.hobbytronics.co.uk/electronic-components/switches-relays/thumb-joystick), and maybe some [breakout boards](http://www.hobbytronics.co.uk/thumb-joystick-breakout) to mount them in
* around 10-14 [pushbutton switches](http://www.hobbytronics.co.uk/electronic-components/switches-relays/push-switch-12mm)
* a small-form-factor MCU such as a [Teensy](http://www.hobbytronics.co.uk/arduino/teensy-v32), [MKR Zero](http://www.hobbytronics.co.uk/arduino/arduino-mkr-zero) or 
[ATmega32U4-based Arduino](http://www.hobbytronics.co.uk/arduino/a-star-32u4-mini)
* an [XBee wireless module](http://www.hobbytronics.co.uk/wireless/zigbee/xbee-1mw-wire-s1) (Bluetooth modules are also available in the same form factor)
* an [XBee breakout board or shield](http://www.hobbytronics.co.uk/wireless/zigbee/xbee-explorer-reg)
* a [regulated 5V battery power supply](http://www.hobbytronics.co.uk/batteries/d24v50f5-5v-step-down-regulator) 
* the [Gamepad_Dfr software library](https://github.com/semuconsulting/Gamepad_DFRobot) could be used with little or no change.

For a rumble effect, you could add:
* a couple of [small DC motors](http://www.hobbytronics.co.uk/motor-control/motors-general/motor-gear-3v) with DIY offset weighted cams to serve as rumble controls
* a small [DC motor driver board](http://www.hobbytronics.co.uk/motor-control/motor-drivers/tb6612fng-driver-board)

You probably wouldn't get much change out of £80, and you would have to construct your own case, but in a Cosplay context you would probably be looking to disguise the controller anyway, so that may open up interesting possibilities.

But if you favour an off-the-shelf 'open-source' approach, the DFRobot controller is worth considering.

#### Serial vs I2C USB Host Adapters

The USB Host adapters or shields available for this kind of application generally support two connection protocols:
 
1) UART ('serial'), which uses the Arduino's hardware UART (TX/RX) ports (e.g. pins D0 & D1 on a UNO) and the standard Serial library.
2) I2C ('two-wire') which uses the Arduino's hardware SDA and SCL ports (e.g. pins A4 & A5 on a UNO) and the Wire library.

There are pros and cons to both options. UART is arguably the easier to use and debug, but if you want to use more than one UART device (e.g. the Robodragon project is also intending to use a 
UART Sound FX board) you may be restricted by the number of hardware UART ports available on your Arduino MCU platform (though this can be partially mitigated by the use of SoftwareSerial libraries).
I2C can in theory support multiple devices over the same ports - each simply needs to be assigned a unique I2C address in software. In practice, there are are occasionally 
hardware or other incompatibilities between different types of I2C-equipped device and it may be necessary to experiment (e.g. with different values of pull-up resistor) to resolve these.
