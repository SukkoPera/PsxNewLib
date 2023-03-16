# PsxNewLib - PlayStation Controller Interface Library for Arduino

![GitHub release (latest by date)](https://img.shields.io/github/v/release/SukkoPera/PsxNewLib)
![GitHub Release Date](https://img.shields.io/github/release-date/SukkoPera/PsxNewLib?color=blue&label=last%20release)
![GitHub commits since latest release (by date)](https://img.shields.io/github/commits-since/SukkoPera/PsxNewLib/latest?color=orange)
[![arduino/arduino-lint-action](https://github.com/SukkoPera/PsxNewLib/actions/workflows/main.yml/badge.svg)](https://github.com/SukkoPera/PsxNewLib/actions/workflows/main.yml)
[![Arduino_CI](https://github.com/SukkoPera/PsxNewLib/actions/workflows/arduino_ci.yml/badge.svg)](https://github.com/SukkoPera/PsxNewLib/actions/workflows/arduino_ci.yml)

PsxNewLib is an Arduino library that allows interfacing controllers designed for the Sony PlayStation and PlayStation 2 with Arduino boards.

When I started developing my [PlayStation to Commodore adapter](https://github.com/SukkoPera/OpenPSX2AmigaPadAdapter), I originally used the [famous library by Bill Porter](http://www.billporter.info/2010/06/05/playstation-2-controller-arduino-library-v1-0/) to interface with the controller. This seemed to work well initially, but then a number of issues surfaced:
- It does not support non-DualShock controllers, which basically rules out all the excellent arcade sticks made for the PlayStation.
- It only supports bit-banging the protocol. This means that it can work on any pins, but since the protocol is essentially SPI, we could let the hardware take care of it, making communication more reliable.
- It just didn't work with some controllers, due to timing issues.
- It looks more like a quick hack at the code level, rather than a polished and maintainable library.

In order to take of these issues, I started working on a new library, and so PsxNewLib was born.

## Features
Currently, PsxNewLib provides access to the status of all digital buttons, analog sticks (on DualShock and later controllers) and analog buttons (on DualShock 2 and later controllers). It also provides functions to enable and disable the analog sticks and buttons.

Since v0.4, it also allows driving the vibration motors available on DualShock and later controllers.

It is compatible with a large number of different controller models, including the GunCon/G-Con light gun by Namco. Please [see below](#compatibility-list) for a list of which have been tested so far.

## Using the Library
First of all, please note that this library depends on [greiman's DigitalIO library](https://github.com/greiman/DigitalIO), which you need to install as well. Unfortunately, the version that is available in the Library Manager has [a bug](https://github.com/greiman/DigitalIO/compare/1.0.0...master) that might cause an error during compilation. Because of this, I recommend not to install it through the Library Manager, but rather to get the master version and install it manually. You can also do that with [my fork](https://github.com/SukkoPera/DigitalIO), which supports a few more platforms.

Moving on to the code, you need to decide whether you want to use the hardware SPI pins or not. According to this, you either have to instantiate a **PsxControllerHwSpi** or **PsxControllerBitBang** object. Then you can just refer to the [example sketches](https://github.com/SukkoPera/PsxNewLib/tree/master/examples/) to learn how to use this library, as the interface should be quite straightforward.

The API has a few rough edges and is not guaranteed to be stable, but any changes will be to make it easier to use.

Among the examples, there is one which will turn any PlayStation controller into a USB one simply by using an Arduino Leonardo or Micro. It is an excellent way to make a cheap adapter and to test the controller and library.

## Wiring the Controller
As the following picture from the [amazing CuriousInventor PS2 Interface Guide](https://store.curiousinventor.com/guides/PS2) shows, PlayStation controllers use 9 pins:

![PS2 Controller Pinout](https://store.curiousinventor.com/wp-content/uploads/2019/09/wiring.jpg)

| Pin # | Signal      | Direction                 | Notes          |
|-------|-------------|---------------------------|----------------|
| 1     | Data        | Controller -> PlayStation | Open Collector |
| 2     | Command     | PlayStation -> Controller |                |
| 3     | Motor Power |                           | 7.5V           |
| 4     | Ground      |                           |                |
| 5     | Power       |                           | 3.6V           |
| 6     | Attention   | PlayStation -> Controller |                |
| 7     | Clock       | PlayStation -> Controller |                |
| 8     | (Unknown)   |                           |                |
| 9     | Acknowledge | Controller -> PlayStation | Open Collector |

**You are advised not to rely on wire colors, but rather on pin positions**. The wires in the image come from an official Sony controller, I expect their colors to be fairly consistent among all Sony controllers, but you shouldn't really trust them.

### TL;DR
Just follow [the PsxControllerShield schematics](https://github.com/SukkoPera/PsxControllerShield/blob/master/doc/schematics.pdf) **but with 1k pull-ups**:
- Power the controller at 3.3V.
- Don't bother with Motor Power unless you wanna rumble.
- Connect all signals with proper level shifting and **1k pull-ups**.

### Power
When plugged in a real PlayStation console, controllers are powered with 3.6V on pin 5. There are (too) many tutorials and videos out there that suggest that powering the controller at 5V is just fine, but the truth is that **it is not**: PlayStation controllers are not made to work at that voltage and **they WILL break** sooner or later. You have been warned.

Now, I know 3.6V regulators aren't exactly common. An LM317 could be used but it requires extra resistors to set the output voltage and as such it is prone to errors. 3.3V seems close enough though, there are plenty of regulators for that voltage and my experience seems to suggest that all PSX controllers work just perfectly with this slight undervoltage, which is definitely safer than powering everything at 5V. Most conveniently, almost all classic Arduinos have an onboard 3.3V regulator. It is generally rated for only "little" current (say 50 mA?), but that will be enough.

Motor Power is not essential if you do not mean to use the rumble feature. If you do, you can derive 7.5V from 9V through an LM317 regulator, unfortunately there are no shortcuts this time... Unless you use one of those wireless controllers that are battery-powered: in this case, don't worry at all about the Motor Power pin.
I recommend using 3.3V power and signal levels. While everything will appear to work fine at 5V, PlayStation controllers are not made to work at that voltage and they will break sooner or later. Many of the tutorials out there ignore this fact, but they really shouldn't.

### Data
If you power the controller at 3.3V, all the I/O lines must also work at that voltage. Most Arduinos work at 5V, thus some level adjustement MUST be done. There are a few ways to achieve this, the easiest of which is probably using those bidirectional 4-channel level shifters you can get cheaply from China. You don't need bidirectionality, strictly speaking, still they are (almost) ideal for this application since they also provide pull-ups for the open-collector outputs and come in sets of 4.

The only issue is that these adapters generally come with 10k pull-up resistors, which are too weak and will create compatibility issues with some controllers. Therefore, **you MUST replace them with 1k ones** or put 1k in parallel to each.

Of course, if you use an Arduino board that works at 3.3V, you won't need any level shifting but **you will still need the pull-up resistors**, as the built-in ones are too weak.

A note on the *Acknowledge* pin: the original library did not use it and this is one of the reasons why it is not compatible with some controllers: it waits for a fixed interval between consecutive bytes instead of checking for the ACK pulse. Since some controllers are slower than others (typically older ones), they might not yet be ready for the next byte if the delay is not well calibrated (and it isn't).

The current version of PsxNewLib does the same, in that it does not use the ACK pin at all, but the delay was calibrated better. However this means that all controllers are polled much slower than they could be. Since this could be easily avoided by polling the ACK pin, **future versions of the library will require it** (the *devel* branch already does), so you are advised to wire it even if it is not necessary for the time being. If you don't want to waste a 4-channel module for a single signal, just connect it directly to an Arduino pin of choice and then connect a 1k resistor between it and 3.3V.

### Arduino Shield
In order to make things as safe and straightforward as possible, **I have designed [an Arduino shield](https://github.com/SukkoPera/PsxControllerShield) that will work perfectly with this library**. Please check it out and use it as your reference for all connections.

## Compatibility List
PsxNewLib aims to be compatible with all devices. I expect this to be the case with all the official controllers produced by Sony. Third-party devices should also work anyway. If you find one that doesn't work, please open an issue and I'll do my best to add support for it.

The following table contains the results of my tests, all done at 3.3V voltage level through my OpenPSX2AmigaPadAdapter:

|Manufacturer|Model                                             |Supported              |Notes                                                                                                                                  |
|------------|--------------------------------------------------|-----------------------|---------------------------------------------------------------------------------------------------------------------------------------|
|Sony        |PlayStation Controller (SCPH-1010)                |![Maybe](img/maybe.png)|Not tested yet                                                                                                                         |
|Sony        |Revised PlayStation Controller (SCPH-1080)        |![Yes](img/yes.png)    |                                                                                                                                       |
|Sony        |Analog Joystick (SCPH-1110)                       |![Yes](img/yes.png)    |Informally known as the *Flightstick*                                                                                                  |
|Sony        |Dual Analog Controller (Japan, SCPH-1150)         |![Maybe](img/maybe.png)|Not tested yet                                                                                                                         |
|Sony        |Dual Analog Controller (USA, SCPH-1180)           |![Maybe](img/maybe.png)|Not tested yet but likely to work, as *SCPH-1180e* does                                                                                |
|Sony        |Dual Analog Controller (Europe, SCPH-1180e)       |![Yes](img/yes.png)    |Controller actually only has *SCPH-1180* on it, but I'm assuming it's the European version since it was bought in Italy                |
|Sony        |DualShock Analog Controller (SCPH-1200)           |![Yes](img/yes.png)    |                                                                                                                                       |
|Sony        |DualShock 2 Analog Controller (SCPH-10010)        |![Yes](img/yes.png)    |                                                                                                                                       |
|Sony        |DualShock Controller for PSOne (SCPH-110)         |![Yes](img/yes.png)    |                                                                                                                                       |
|Asciiware   |Arcade Stick (SCEH-0002)                          |![Yes](img/yes.png)    |                                                                                                                                       |
|Logitech    |Cordless Action (G-X2D11)                         |![Yes](img/yes.png)    |                                                                                                                                       |
|Namco       |Arcade Stick (NPC-102 (SLEH-0004))                |![Yes](img/yes.png)    |                                                                                                                                       |
|Namco       |neGcon (NPC-101)                                  |![Yes](img/yes.png)    |Since v0.3                                                                                                                             |
|Namco       |JogCon (NPC-105)                                  |![Yes](img/yes.png)    |Since v0.3                                                                                                                             |
|Namco       |G-Con/GunCon                                      |![Yes](img/yes.png)    |Since v0.4, see the *GunconAbsMouse* example for details - Tested by @sonik-br                                                         |
|Taito       |Densha de Go! Two-Handle Controller (SLPH-00051)  |![Yes](img/yes.png)    |Tested by @tylau0                                                                                                                      |
|Taito       |Densha de Go! One-Handle Controller (TCPP-20001)  |![Yes](img/yes.png)    |Since v0.4 - Tested by @tylau0                                                                                                         |
|EastVita    |Wireless Controller                               |![Yes](img/yes.png)    |Chinese knock-off, cheap but with surprising quality, pretty similar to the Lynxmotion controller, probably goes under other names, too|

## Debugging
If you have problems, uncomment the `DUMP_COMMS` #define in [PsxNewLib.h](https://github.com/SukkoPera/PsxNewLib/blob/master/src/PsxNewLib.h#L33) and watch your serial monitor.

## Releases
If you want to use this library, you are recommended to get [the latest release](https://github.com/SukkoPera/PsxNewLib/releases) rather than the current git version, as the latter might be under development and is not guaranteed to be working.

Every release is accompanied by any relevant notes about it, which you are recommended to read carefully.

## License
PsxNewLib is released under the GNU General Public License (GPL) v3. If you make any modifications to the library, **you must** contribute them back.

PsxNewLib is provided to you ‘as is’ and without any express or implied warranties whatsoever with respect to its functionality, operability or use, including, without limitation, any implied warranties of merchantability, fitness for a particular purpose or infringement. We expressly disclaim any liability whatsoever for any direct, indirect, consequential, incidental or special damages, including, without limitation, lost revenues, lost profits, losses resulting from business interruption or loss of data, regardless of the form of action or legal theory under which the liability may be asserted, even if advised of the possibility or likelihood of such damages.

## Thanks
- Bill Porter for the original library.
- CuriousInventor for their [excellent interfacing guide](https://store.curiousinventor.com/guides/PS2).
- Matheus Fraguas (@sonik-br) for helping getting the G-Con/GunCon supported.
- Eddie Lau (@tylau0) for helping with the *Densha de Go!* controllers.
- Kate (@katemonster33) for contributing Rumble support.
- All the other guys who helped understand how the PSX controller protocol works.
