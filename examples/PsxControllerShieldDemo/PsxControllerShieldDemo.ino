/*******************************************************************************
 * This file is part of PsxNewLib.                                             *
 *                                                                             *
 * Copyright (C) 2019-2020 by SukkoPera <software@sukkology.net>               *
 *                                                                             *
 * PsxNewLib is free software: you can redistribute it and/or                  *
 * modify it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * PsxNewLib is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU General Public License for more details.                                *
 *                                                                             *
 * You should have received a copy of the GNU General Public License           *
 * along with PsxNewLib. If not, see http://www.gnu.org/licenses.              *
 *******************************************************************************
 *
 * This example is very similar to DumpButtonsHwSpi, it only has some slight
 * modifications so that it can be used to test my PsxControllerShield
 * (https://github.com/SukkoPera/PsxControllerShield). It will dump to serial
 * whatever is done on a PSX controller and will light the available leds
 * according to various events.
 *
 * It is an excellent way to test that all buttons/sticks are read correctly,
 * and thus that both the controller and the shield are working fine.
 *
 * It's missing support for analog buttons, that will come in the future.
 *
 * Note that on Leonardo and other boards with a CDC USB serial port, all three
 * leds will blink at startup in the serial port is not connected. Just open
 * your serial monitor to go on :).
 */

// PsxControllerShield connects controller to HW SPI port through ICSP connector
#include <PsxControllerHwSpi.h>
#include <DigitalIO.h>

#include <avr/pgmspace.h>
typedef const __FlashStringHelper * FlashStr;
typedef const byte* PGM_BYTES_P;
#define PSTR_TO_F(s) reinterpret_cast<const __FlashStringHelper *> (s)

/** \brief Pin used for Controller Attention (ATTN)
 *
 * This pin makes the controller pay attention to what we're saying. The shield
 * has pin 10 wired for this purpose.
 */
const byte PIN_PS2_ATT = 10;

/** \brief Pin for Controller Presence Led
 *
 * This led will light up steadily whenever a controller is detected and be off
 * otherwise.
 */
const byte PIN_HAVECONTROLLER = 8;

/** \brief Pin for Button Press Led
 *
 * This led will light up whenever a button is pressed on the controller.
 */
const byte PIN_BUTTONPRESS = 7;

/** \brief Pin for Analog Movement Detection
 *
 * This led will light up whenever the left or right analog sticks are moved.
 */
const byte PIN_ANALOG = 6;

/** \brief Dead zone for analog sticks
 *  
 * If the analog stick moves less than this value from the center position, it
 * is considered still.
 * 
 * \sa ANALOG_IDLE_VALUE
 */
const byte ANALOG_DEAD_ZONE = 50U;

PsxControllerHwSpi<PIN_PS2_ATT> psx;

boolean haveController = false;

const char buttonSelectName[] PROGMEM = "Select";
const char buttonL3Name[] PROGMEM = "L3";
const char buttonR3Name[] PROGMEM = "R3";
const char buttonStartName[] PROGMEM = "Start";
const char buttonUpName[] PROGMEM = "Up";
const char buttonRightName[] PROGMEM = "Right";
const char buttonDownName[] PROGMEM = "Down";
const char buttonLeftName[] PROGMEM = "Left";
const char buttonL2Name[] PROGMEM = "L2";
const char buttonR2Name[] PROGMEM = "R2";
const char buttonL1Name[] PROGMEM = "L1";
const char buttonR1Name[] PROGMEM = "R1";
const char buttonTriangleName[] PROGMEM = "Triangle";
const char buttonCircleName[] PROGMEM = "Circle";
const char buttonCrossName[] PROGMEM = "Cross";
const char buttonSquareName[] PROGMEM = "Square";

const char* const psxButtonNames[PSX_BUTTONS_NO] PROGMEM = {
	buttonSelectName,
	buttonL3Name,
	buttonR3Name,
	buttonStartName,
	buttonUpName,
	buttonRightName,
	buttonDownName,
	buttonLeftName,
	buttonL2Name,
	buttonR2Name,
	buttonL1Name,
	buttonR1Name,
	buttonTriangleName,
	buttonCircleName,
	buttonCrossName,
	buttonSquareName
};

byte psxButtonToIndex (PsxButtons psxButtons) {
	byte i;

	for (i = 0; i < PSX_BUTTONS_NO; ++i) {
		if (psxButtons & 0x01) {
			break;
		}

		psxButtons >>= 1U;
	}

	return i;
}

FlashStr getButtonName (PsxButtons psxButton) {
	FlashStr ret = F("");
	
	byte b = psxButtonToIndex (psxButton);
	if (b < PSX_BUTTONS_NO) {
		PGM_BYTES_P bName = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(psxButtonNames[b])));
		ret = PSTR_TO_F (bName);
	}

	return ret;
}

void dumpButtons (PsxButtons psxButtons) {
	static PsxButtons lastB = 0;

	if (psxButtons != lastB) {
		lastB = psxButtons;     // Save it before we alter it
		
		Serial.print (F("Pressed: "));

		for (byte i = 0; i < PSX_BUTTONS_NO; ++i) {
			byte b = psxButtonToIndex (psxButtons);
			if (b < PSX_BUTTONS_NO) {
				PGM_BYTES_P bName = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(psxButtonNames[b])));
				Serial.print (PSTR_TO_F (bName));
			}

			psxButtons &= ~(1 << b);

			if (psxButtons != 0) {
				Serial.print (F(", "));
			}
		}

		Serial.println ();
	}
}

void dumpAnalog (const char *str, const int8_t x, const int8_t y) {
	Serial.print (str);
	Serial.print (F(" analog: x = "));
	Serial.print (x);
	Serial.print (F(", y = "));
	Serial.println (y);
}

// We like analog sticks to return something in the [-127, +127] range
boolean rightAnalogMoved (int8_t& x, int8_t& y) {
	boolean ret = false;
	byte rx, ry;
	
	if (psx.getRightAnalog (rx, ry)) {				// [0 ... 255]
		int8_t deltaRX = rx - ANALOG_IDLE_VALUE;	// [-128 ... 127]
		if (abs (deltaRX) > ANALOG_DEAD_ZONE) {
			x = deltaRX;
			if (x == -128)
				x = -127;
			ret = true;
		} else {
			x = 0;
		}
		
		int8_t deltaRY = ry - ANALOG_IDLE_VALUE;
		if (abs (deltaRY) > ANALOG_DEAD_ZONE) {
			y = deltaRY;
			if (y == -128)
				y = -127;
			ret = true;
		} else {
			y = 0;
		}
	}

	return ret;
}

boolean leftAnalogMoved (int8_t& x, int8_t& y) {
	boolean ret = false;
	byte lx, ly;
	
	if (psx.getLeftAnalog (lx, ly)) {				// [0 ... 255]
		if (psx.getProtocol () != PSPROTO_NEGCON && psx.getProtocol () != PSPROTO_JOGCON) {
			int8_t deltaLX = lx - ANALOG_IDLE_VALUE;	// [-128 ... 127]
			uint8_t deltaLXabs = abs (deltaLX);
			if (deltaLXabs > ANALOG_DEAD_ZONE) {
				x = deltaLX;
				if (x == -128)
					x = -127;
				ret = true;
			} else {
				x = 0;
			}
			
			int8_t deltaLY = ly - ANALOG_IDLE_VALUE;
			uint8_t deltaLYabs = abs (deltaLY);
			if (deltaLYabs > ANALOG_DEAD_ZONE) {
				y = deltaLY;
				if (y == -128)
					y = -127;
				ret = true;
			} else {
				y = 0;
			}
		} else {
			// The neGcon and JogCon are more precise and work better without any dead zone
			x = lx, y = ly;
		}
	}

	return ret;
}


// Controller Type
const char ctrlTypeUnknown[] PROGMEM = "Unknown";
const char ctrlTypeDualShock[] PROGMEM = "Dual Shock";
const char ctrlTypeDsWireless[] PROGMEM = "Dual Shock Wireless";
const char ctrlTypeGuitHero[] PROGMEM = "Guitar Hero";
const char ctrlTypeOutOfBounds[] PROGMEM = "(Out of bounds)";

const char* const controllerTypeStrings[PSCTRL_MAX + 1] PROGMEM = {
	ctrlTypeUnknown,
	ctrlTypeDualShock,
	ctrlTypeDsWireless,
	ctrlTypeGuitHero,
	ctrlTypeOutOfBounds
};


// Controller Protocol
const char ctrlProtoUnknown[] PROGMEM = "Unknown";
const char ctrlProtoDigital[] PROGMEM = "Digital";
const char ctrlProtoDualShock[] PROGMEM = "Dual Shock";
const char ctrlProtoDualShock2[] PROGMEM = "Dual Shock 2";
const char ctrlProtoFlightstick[] PROGMEM = "Flightstick";
const char ctrlProtoNegcon[] PROGMEM = "neGcon";
const char ctrlProtoJogcon[] PROGMEM = "Jogcon";
const char ctrlProtoOutOfBounds[] PROGMEM = "(Out of bounds)";

const char* const controllerProtoStrings[PSPROTO_MAX + 1] PROGMEM = {
	ctrlProtoUnknown,
	ctrlProtoDigital,
	ctrlProtoDualShock,
	ctrlProtoDualShock2,
	ctrlProtoFlightstick,
	ctrlProtoNegcon,
	ctrlProtoJogcon,
	ctrlTypeOutOfBounds
};

 
void setup () {
	fastPinMode (PIN_HAVECONTROLLER, OUTPUT);
	fastPinMode (PIN_BUTTONPRESS, OUTPUT);
	fastPinMode (PIN_ANALOG, OUTPUT);
	
	delay (300);

	Serial.begin (115200);
	while (!Serial) {
		// Wait for serial port to connect on Leonardo boards
		fastDigitalWrite (PIN_HAVECONTROLLER, (millis () / 333) % 2);
		fastDigitalWrite (PIN_BUTTONPRESS, (millis () / 333) % 2);
		fastDigitalWrite (PIN_ANALOG, (millis () / 333) % 2);
	}
	Serial.println (F("Ready!"));
}
 
void loop () {
	static int8_t slx, sly, srx, sry;
	
	fastDigitalWrite (PIN_HAVECONTROLLER, haveController);
	
	if (!haveController) {
		if (psx.begin ()) {
			Serial.println (F("Controller found!"));
			delay (300);
			if (!psx.enterConfigMode ()) {
				Serial.println (F("Cannot enter config mode"));
			} else {
				PsxControllerType ctype = psx.getControllerType ();
				PGM_BYTES_P cname = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerTypeStrings[ctype < PSCTRL_MAX ? static_cast<byte> (ctype) : PSCTRL_MAX])));
				Serial.print (F("Controller Type is: "));
				Serial.println (PSTR_TO_F (cname));

				if (!psx.enableAnalogSticks ()) {
					Serial.println (F("Cannot enable analog sticks"));
				}
				
				if (!psx.enableAnalogButtons ()) {
					Serial.println (F("Cannot enable analog buttons"));
				}
				
				if (!psx.exitConfigMode ()) {
					Serial.println (F("Cannot exit config mode"));
				}
			}

			psx.read ();		// Make sure the protocol is up to date
			PsxControllerProtocol proto = psx.getProtocol ();
			PGM_BYTES_P pname = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerProtoStrings[proto < PSPROTO_MAX ? static_cast<byte> (proto) : PSPROTO_MAX])));
			Serial.print (F("Controller Protocol is: "));
			Serial.println (PSTR_TO_F (pname));

			haveController = true;
		}
	} else {
		if (!psx.read ()) {
			Serial.println (F("Controller lost :("));
			haveController = false;
		} else {
			fastDigitalWrite (PIN_BUTTONPRESS, !!psx.getButtonWord ());
			dumpButtons (psx.getButtonWord ());

			int8_t lx = 0, ly = 0;
			leftAnalogMoved (lx, ly);
			if (lx != slx || ly != sly) {
				dumpAnalog ("Left", lx, ly);
				slx = lx;
				sly = ly;
			}

			int8_t rx = 0, ry = 0;
			rightAnalogMoved (rx, ry);
			if (rx != srx || ry != sry) {
				dumpAnalog ("Right", rx, ry);
				srx = rx;
				sry = ry;
			}

			fastDigitalWrite (PIN_ANALOG, lx != 0 || ly != 0 || rx != 0 || ry != 0);
		}
	}

	// Only poll "once per frame" ;)
	delay (1000 / 60);
}
