# Gamepad-style Wireless Controller Options for Arduino Projects

A variety of gamepad-style wireless remote control configurations are available for Arduino projects, some of which are illustrated below. I have experimented with most of these at various point and 
have endeavoured to develop (or adapt from existing sources) a [simple, standardised set of Arduino gamepad software libraries](https://github.com/semuconsulting) to 
allow end users to adopt different configurations more or less interchangeably. See the README files in these libraries for further details.

PS3 USB/XBee --> XBee   | PS3/4 --> Bluetooth    | DFRobot XBee --> XBee
----------------------- | :--------------------: | :--------------------:
![ps3](/docs/Configurationps3.png) | ![ps4bt](/docs/Configurationps4bt.png) | ![dfr](/docs/Configurationdfr.png)

## Standard PS3/PS4 Controller
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

### Serial vs I2C USB Host Adapters

The USB Host adapters or shields available for this kind of application generally support the following connection protocols:
 
1) UART ('serial'), which uses the Arduino's hardware UART (TX/RX) ports (e.g. pins D0 & D1 on a UNO) and the standard Serial library.
2) I2C ('two-wire') which uses the Arduino's hardware SDA and SCL ports (e.g. pins A4 & A5 on a UNO) and the Wire library.

There are pros and cons to both options. UART is arguably the easier to use and debug, but if you want to use more than one UART device (e.g. the Robodragon project is also intending to use a 
UART Sound FX board) you may be restricted by the number of hardware UART ports available on your Arduino MCU platform (though this can be partially mitigated by the use of SoftwareSerial libraries).
I2C can in theory support multiple devices over the same ports - each simply needs to be assigned a unique I2C address in software. In practice, there are are occasionally 
hardware or other incompatibilities between different types of I2C-equipped device and it may be necessary to experiment (e.g. with different values of pull-up resistor) to resolve these.

## Third-party Arduino-based Wireless Gamepad Controller
An alternative option is something like the [DFRobot PS3-style controller](https://www.dfrobot.com/product-858.html) which embeds a programmable Arduino Leonardo-compatible MCU with 
a slot for an XBee radio module (either RF or Bluetooth). This has the advantage that you can program the transmitting MCU yourself and therefore have complete control over its 
functionality, and you can select different XBee platforms depending on your wireless needs.

The disadvantages of the DFRobot controller option are a) it uses hobby-grade components and the build quality and control responsiveness are nothing like as good as a genuine 
PS3/PS4 controller, b) it's not particularly cheap (around £50 in the UK, plus around £23 for the XBee module, but additional import charges may be incurred if sourced from China) and c) the documentation is poor.

But if you favour an off-the-shelf 'open-source' approach, the DFRobot controller is worth considering.

## DIY Wireless Controller

You could, at a push, build your own equivalent wireless 'gamepad' using components available from most electronic hobby stores:
 
* a couple of [thumb joysticks](http://www.hobbytronics.co.uk/electronic-components/switches-relays/thumb-joystick), and maybe some [breakout boards](http://www.hobbytronics.co.uk/thumb-joystick-breakout) to mount them in
* around 10-14 [pushbutton switches](http://www.hobbytronics.co.uk/electronic-components/switches-relays/push-switch-12mm)
* a small-form-factor MCU such as a [Teensy](http://www.hobbytronics.co.uk/arduino/teensy-v32), [MKR Zero](http://www.hobbytronics.co.uk/arduino/arduino-mkr-zero) or 
[ATmega32U4-based Arduino](http://www.hobbytronics.co.uk/arduino/a-star-32u4-mini)
* an [XBee wireless module](http://www.hobbytronics.co.uk/wireless/zigbee/xbee-1mw-wire-s1). Bluetooth and WiFi modules are also available in the same form factor. There are 
cheaper 433MHz-band radio transceivers available, but the XBee modules are largely self-contained, have good range, are well documented and easy to use and are generally bomb-proof.
* an [XBee breakout board or shield](http://www.hobbytronics.co.uk/wireless/zigbee/xbee-explorer-reg)
* a [regulated 5V battery power supply](http://www.hobbytronics.co.uk/batteries/d24v50f5-5v-step-down-regulator) 
* the [Gamepad_Dfr software library](https://github.com/semuconsulting/Gamepad_DFRobot) could be used with little or no change.

For a rumble effect, you could add:
* a couple of [small DC motors](http://www.hobbytronics.co.uk/motor-control/motors-general/motor-gear-3v) with DIY offset weighted cams to serve as rumble controls
* a small [DC motor driver board](http://www.hobbytronics.co.uk/motor-control/motor-drivers/tb6612fng-driver-board)

You probably wouldn't get much change out of £80, and you would have to construct your own case, but in a Cosplay context you would probably be looking to disguise the controller anyway, so that may open up interesting possibilities.

## IR Remote Controller

For simple short-range line-of-sight applications (up to around 8m), an [IR remote control and LED receiver](http://www.hobbytronics.co.uk/wireless/hx1838-infra-red-remote) offers a cheap and cheerful option,
though you'll obviously get limited accuracy in terms of x/y axis control. Serviceable IR remotes are often supplied with [Arduino educational kits](https://www.amazon.co.uk/dp/B01IUY62RM/ref=sxnav_sxwds-bovbp-i_lm_l_1?pf_rd_m=A3P5ROKL5A1OLE&pf_rd_p=42635de0-01bc-4eee-8442-f1d8718d081a&pd_rd_wg=XtrBl&pf_rd_r=2MJCQC2EY319EQ0F7MBE&pf_rd_s=desktop-sx-nav&pf_rd_t=301&pd_rd_i=B01IUY62RM&pd_rd_w=qLPLV&pf_rd_i=arduino+starter+kit&pd_rd_r=92f8422a-e765-4068-b296-5b459debca44&ie=UTF8&qid=1533932358&sr=1).

