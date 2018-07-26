Zygos Mask Lights project

ZML WSSERVER
============

### Summary

Control WS2812 leds with a ESP8266 based card (NodeMCU, WeMos D1,...) connected to a WiFi network, by commands sent by a HTML interface thru WebSockets.
This part concerns the esp8266 card program.


Abstract
--------

This repository is part of the Zygos Mask Lights project, inserting leds into masks and control them over a wifi network.

This part contains the code for the microcontroller (a ESP8266 card, NodeMCU or WeMos D1...) listening the network via WebSocket, and command the leds inside the mask (WS2812).

The other parts are:
- [zml_webcontrol](https://github.com/joliclic/zml_webcontrol): an html page for sending commands via WebSocket.
- [zml_nodemcu_box](https://github.com/joliclic/zml_nodemcu_box): plan for a box for the NodeMCU card, made with MDF cutted by laser.


What is ZYGOS MASKS LIGHTS ?
----------------------------

The [Zygos Brass Band](http://zygos.fr) uses for its show named "Trybz" masks made with polypropylene sheet cutted with laser (original plans by Wintercroft). This project add some leds (WS2812 plugged into a ESP8266 based card, like NodeMCU or WeMos D1) inside them, and control them with a tablet/smartphone over a wifi network via WebSocket.


Organisation of this code
-------------------------

The code is compiled and loaded inside a NodeMCU with the Arduino IDE. Each masks share parts of code, but need some specific code (led mapping, ip, ...).
Arduino IDE only wants an .ino project inside a unique folder with the same name for the folder and the .ino file.
So a build system based on Node.js is used to generate a ino project for each masks from common and specific code.

You need to define the router parameters in a file named `router_config.h` inside the `common` folder (use `router_config.default.h` as an example).
And you can adapt wifi parameters for each cards in the `wifi_host_config.h` file.

Then, `node build.js` will generate an ino file for each mask in their own directory inside the `generated` directory.


Dependencies
------------

You need [Node.js](https://nodejs.org) to generate the ino files. And install the node dependencies for this project (`npm install` from the main folder).  
And you need [Arduino Ide](https://www.arduino.cc/en/main/software), with the [esp8266 library](https://github.com/esp8266/Arduino) (at least version 2.4) installed (see the link for installation).
On the date I wrote these lines, the latest stable release of the esp8266 library is 2.3.0, so you need to use the 2.4.0-rc2 pre-release. You can install it with the Board Manager like the stable release, but the following link:  
`https://github.com/esp8266/Arduino/releases/download/2.4.0-rc2/package_esp8266com_index.json`  
Alternatively you can use the git version, see their doc.

Then you can open each generated ino file with the ide and load them inside the card. Select your model as CardType (ex: "NodeMCU 1.0 (ESP-12E Module)" , "WeMos D1 R2 & Mini",...) .


How to use it ?
---------------

The leds are commanded by the D2 pin of the NodeMCU (with a 470 ohm resistance near the leds), and are powered by the 3.3V and ground pins of the NodeMCU. Yes, the WS2812 needs 5V (data signal and power), and the NodeMCU is 3.3v only. It works as is for us ;) . We use an external battery for smartphone directly plugged in the usb port of the card.

When the card is on power, a first white flash of all the leds should happen after the boot, then a second one when the card is connected to the network.
Now you need the HTML page to send commands to the lights, this is the [zml_webcontrol](https://github.com/joliclic/zml_webcontrol) part of this project.


License
-------

All the code is under the MIT license.


---

video:

[![link to a demonstration video](https://i.ytimg.com/vi/-qXGG1oTjpg/hqdefault.jpg)](https://youtu.be/-qXGG1oTjpg "Zygos Mask Lights video link")

photos:

![photo 1](https://github.com/joliclic/zml_wsserver/wiki/images/340px-zygos_mask_1.jpeg)

