# Robodragon
"Animatronix" cosplay dragon project

This originated as a cosplay project to create a life-size (i.e. about the size of a cat) 'animatronix' dragon based on the Terry Pratchett character 
Vinny aka Lord Mountjoy Quickfang Winterforth IV, who appears in the Discworld volume Guards! Guards!

![discworld dragon](/discworlddragon.png)

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

![provisional wiring diagram](/wiringdiagram.png)

## Current Status

1. Favouring the 32-bit Teensy 3.x microcontroller (MCU), mainly due to small form factor and excellent processor speed and program memory capacity. 
The main downside to the Teensy platform is that the header pins have to be soldered by the end user, which is not a task for the faint-hearted. 
If the soldering is daunting, the code also works on most 32-bit Arduino platforms such as the Due or Zero, but a standard 8-bit Arduino UNO does have have enough grunt or memory to 
support all planned functionality.
2. Key challenge has been getting the OLED displays to work reliably as part of a moving assembly. The SPI protocols used by these displays are only intended for transmission 
over very short and rigid tracks (typically a few cm at most), so this project is approaching the limit of what is practically feasible using SPI devices. [Electro-mechanical 
animatronic eyes](https://www.pinterest.co.uk/pin/562035228467357323/) were considered, but these would have made the head unfeasibly large and you lose the ability to 
alter the look and feel programatically. The 'cartoon like' look of the OLED displays seemed appropriate for the Vinny character.
3. Arduino code libraries developed for the SPI displays and a variety of wireless gamepad controllers. These can be found at https://github.com/semuconsulting/
4. Mechanical framework for head and wings complete and tested.
5. Remaining task is to incorporate electro-mechanics into a convincing dragon 'body' and make the whole thing robust and reliable enough for the rigours of cosplay.

## Work in Progress Pictures

![robodragon head wiring matrix 2](/dragonheadwiringmatrix2.jpg) ![robodragon head wiring matrix 3](/dragonheadwiringmatrix3.jpg)

![Teensy shield 1](/teensyshield1.jpg) ![Teensy shield 2](/teensyshield2.jpg)

## Gamepad Remote Control Options

A variety of gamepad-style remote control options are available. I have experimented with most of these at various point and have endeavoured to develop (or 
adapt from existing sources) a [simple, standardised set of Arduino gamepad software libraries](https://github.com/semuconsulting) to allow end users 
to adopt different hardware platforms more or less interchangeably. See the README files in these libraries for further details.

### Standard PS3/PS4 Controller
Perhaps the most reliable and intuitive option is to use a standard PS3 or PS4 controller (or the XBox equivalent) in conjunction with some form of wireless transceiver - either Bluetooth or 
a longer range RF system like [XBee](https://en.wikipedia.org/wiki/XBee). Using Bluetooth reduces the overall parts count and cost, as both PS3 and PS4 
controllers already incorporate Bluetooth transmitters. The key disadvantages of Bluetooth are a) limited range, and b) the need to 'pair' the controller 
with the Bluetooth receiver, which can be unreliable in an Arduino hardware context. 

If Bluetooth proves unreliable, it is also possible to use a PS3 controller connected via a physical USB cable to a [portable USB Host adapter and XBee transmitter](http://www.hobbytronics.co.uk/usb-host-xbee-shield), 
though this does increase the parts count and cost and is a little more cumbersome to carry around. I've personally found this to be the most reliable option.

Another disadvantage with the PS3/PS4 option is that these are proprietary platforms and you are generally reliant on closed-source, reverse-engineered 
USB software libraries to interpret the PS3/PS4 output. On some hobbyist platforms, the software is only available in compiled binary (hex) format - you can't actually see or debug the code. 
There is an active [open-source project](https://github.com/felis/UHS30) working on opening up such USB protocols, and also allowing the use of native USB ports provided by some 
of the more capable Arduino platforms (such as the Due or Teensy) - obviating the need to use an external USB Host adapter or shield - but at time of writing this was still at 
an early Alpha stage.

There are third-party PS3/PS4 clones but the build quality often leaves a lot to be desired. Genuine second-hand PS3 controllers in good condition can 
be found on eBay for around £15 and I'd recommend sticking with the genuine article.

### Third-party Arduino-based Gamepad Controller
An alternative option is something like the [DFRobot PS3-style controller](https://www.dfrobot.com/product-858.html) which embeds a programmable Arduino Leonardo-compatible MCU with 
a slot for an XBee-compatible radio module (either RF or Bluetooth). This has the advantage that you can program the MCU yourself and therefore have complete control over its 
functionality, and you can select different XBee platforms depending on your wireless needs.

The disadvantages of the DFRobot controller option are a) it uses hobby-grade components and the build quality and control responsiveness are nothing like as good as a genuine 
PS3/PS4 controller, b) it's not particularly cheap (and may need to be delivered from China, incurring additional import charges) and c) it's not well documented.

But if you favour a completely 'open-source' approach, the DFRobot controller is worth considering.

#### Serial vs I2C USB Host Adapters

The USB Host adapters available in this context generally support two connection protocols:
 
1) UART ('serial'), which uses the Arduino's hardware UART (TX/RX) ports (e.g. pins D0 & D1 on a UNO) and the standard Serial library.
2) I2C ('two-wire') which uses the Arduino's hardware SDA and SCL ports (e.g. pins A4 & A5 on a UNO) and the Wire library.

There are pros and cons to both options. UART is arguably the easier to use and debug, but if you want to use more than one UART device (e.g. the Robodragon project is also intending to use a 
UART Sound FX board) you may be restricted by the number of hardware UART ports available on your Arduino MCU platform (though this can be partially mitigated by the use of SoftwareSerial libraries).
I2C can in theory support multiple devices over the same ports - each simply needs to be assigned a unique I2C address in software. In practice, there are are occasionally 
hardware or other incompatibilities between different I2C-equipped devices and it may be necessary to experiment (e.g. with different values of pull-up resistor) to resolve these.
