# PsxNewLib
Playstation controller interface library for Arduino

# Playstation controller interface library for Arduino
PsxNewLib is a Arduino library that allows interfacing many controllers designed for the Sony PlayStation and PlayStation 2 with Arduino boards.

When I started developing my [PlayStation to Commodore adapter](https://github.com/SukkoPera/OpenPSX2AmigaPadAdapter), I originally used the [famous library by Bill Porter](http://www.billporter.info/2010/06/05/playstation-2-controller-arduino-library-v1-0/) to interface with the controller. This seemed to well initially, but then a number of issues surfaced:
- It does not support non-DualShock controllers, which basically rules out all the excellent arcade sticks made for the PlayStation.
- It only supports bit-banging the protocol. This means that it can work on any pins, but since the protocol is essentially SPI, which is supported in hardware by all ATmega's, we could exploit the hardware and make communication more reliable.
- It just didn't work with some controllers, due to timing issue.
- It looks more like a quick hack at the code level, rather than a well-thought-out and polished library.

So I started developing PsxNewLib, in order to take of all these issues.

## Wiring the Controller
Follow the pinout in the following picture from the [amazing CuriousInventor PS2 Interface Guide](https://store.curiousinventor.com/guides/PS2):

![PS2 Controller Pinout](https://store.curiousinventor.com/wp-content/uploads/2019/09/wiring.jpg)

**You are advised not to rely on wire colors, but rather on pin positions**. The wires in the image come from an official Sony controller, I expect their colors to be fairly consistent among all Sony controllers, but you shouldn't really trust them.

Lynxmotion sells [this nice breakout connector](https://www.robotshop.com/en/ps2-connector-breakout-board.html) which makes things much easier.

## Using the Library
First of all you must decide whether you want to use the hardware SPI pins or not. According to this, you either have to instantiate a **PsxControllerHwSpi** or **PsxControllerBitBang** object. Then you can just refer to the [example sketches](https://github.com/SukkoPera/PsxNewLib/tree/autoshift/examples/) to learn how to use this library, as the interface should be quite straightforward.

## Compatibility List
PsxNewLib aims to be compatible with all devices. I expect this to be the case with all the official controllers produced by Sony. Third-party devices should also work anyway. If you find one that doesn't work, please open an issue and I'll do my best to add support for it.

|Manufacturer|Model                                          |Supported|Notes                                |
|------------|-----------------------------------------------|---------|-------------------------------------|
|Sony        |PlayStation Controller (SCPH-1010)             |![Yes](img/yes.png)    |                                     |
|Sony        |Revised PlayStation Controller (SCPH-1080)     |         |                                     |
|Sony        |Analog Joystick (SCPH-1110)                    |![Yes](img/yes.png)    |Informally known as the *Flighstick*.|
|Sony        |Dual Analog Controller (Japan, SCPH-1150)      |         |                                     |
|Sony        |Dual Analog Controller (USA, SCPH-1180)        |         |                                     |
|Sony        |Dual Analog Controller (Europe, SCPH-1180e)    |         |                                     |
|Sony        |DualShock Analog Controller (SCPH-1200)        |![Yes](img/yes.png)    |                                     |
|Sony        |DualShock 2 Analog Controller (SCPH-10010)     |![Yes](img/yes.png)    |                                     |
|Sony        |DualShock Controller for PSOne (SCPH-110)      |         |                                     |
|Asciiware   |Arcade Stick                                   |![Yes](img/yes.png)    |                                     |
|Namco       |Arcade Stick                                   |![Yes](img/yes.png)    |                                     |
|Logitech    |Cordless Action                                |         |                                     |
|Namco       |Negcon (NPC-101)                               |No       |Looking for one                      |
|EastVita    |Wireless Controller                            |![Yes](img/yes.png)    |Chinese knock-off, cheap but with surprising quality, pretty similar to the Lynxmotion controller, probably goes under other names, too |

## Debugging
If you have problems, uncomment the DUMP_COMMS #define in [https://github.com/SukkoPera/PsxNewLib/blob/autoshift/src/PsxNewLib.h#L33](PsxNewLib.h) and watch your serial monitor.

## License
PsxNewLib is released under the GNU General Public License (GPL) v3. If you make any modifications to the board, **you must** contribute them back.

PsxNewLib is provided to you ‘as is’ and without any express or implied warranties whatsoever with respect to its functionality, operability or use, including, without limitation, any implied warranties of merchantability, fitness for a particular purpose or infringement. We expressly disclaim any liability whatsoever for any direct, indirect, consequential, incidental or special damages, including, without limitation, lost revenues, lost profits, losses resulting from business interruption or loss of data, regardless of the form of action or legal theory under which the liability may be asserted, even if advised of the possibility or likelihood of such damages.

## Thanks
- Bill Porter for the original library.
- CuriousInventor for their [excellent interfacing guide](https://store.curiousinventor.com/guides/PS2).
- All the other guys who helped understand how the PSX controller protocol works.
