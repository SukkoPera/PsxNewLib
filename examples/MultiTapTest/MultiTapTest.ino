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
 * This sketch will dump to serial whatever is done on a PSX controller. It is
 * an excellent way to test that all buttons/sticks are read correctly.
 *
 * It's missing support for analog buttons, that will come in the future.
 *
 * This example drives the controller through the hardware SPI port, so pins are
 * fixed and depend on the board/microcontroller being used. For instance, on an
 * Arduino Uno connections must be as follows:
 *
 * CMD: Pin 11
 * DATA: Pin 12
 * CLK: Pin 13
 *
 * Any pin can be used for ATTN, but please note that most 8-bit AVRs require
 * the HW SPI SS pin to be kept as an output for HW SPI to be in master mode, so
 * using that pin for ATTN is a natural choice. On the Uno this would be pin 10.
 *
 * It also works perfectly on OpenPSX2AmigaPadAdapter boards (as it's basically
 * a modified Uno).
 *
 * There is another similar one using a bitbanged protocol implementation that
 * can be used on any pins/board.
 */

#include <DigitalIO.h>
#include <PsxDriverHwSpi.h>
//~ #include <PsxDriverBitBang.h>
#include <PsxMultiTap.h>

#include <avr/pgmspace.h>
typedef const __FlashStringHelper * FlashStr;
typedef const byte* PGM_BYTES_P;
#define PSTR_TO_F(s) reinterpret_cast<const __FlashStringHelper *> (s)

// This can be changed freely but please see above
const byte PIN_PS2_ATT = 10;

// These can be changed freely when using the bitbanged protocol
const byte PIN_PS2_CMD = 11;
const byte PIN_PS2_DAT = 12;
const byte PIN_PS2_CLK = 13;

const byte PIN_HAVEMULTITAP = 8;
//~ const byte PIN_BUTTONPRESS = A0;

const char ctrlProto00[] PROGMEM = "Unknown";
const char ctrlProto01[] PROGMEM = "Digital";
const char ctrlProto02[] PROGMEM = "Dual Shock";
const char ctrlProto03[] PROGMEM = "Dual Shock 2";
const char ctrlProto04[] PROGMEM = "Flight Stick";
const char ctrlProto05[] PROGMEM = "neGcon";
const char ctrlProto06[] PROGMEM = "JogCon";
const char ctrlProto99[] PROGMEM = "(Out of bounds)";

const char* const controllerProtocolStrings[PSPROTO_MAX + 1] PROGMEM = {
	ctrlProto00,
	ctrlProto01,
	ctrlProto02,
	ctrlProto03,
	ctrlProto04,
	ctrlProto05,
	ctrlProto06,
	ctrlProto99
};

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

void dumpButtons (const byte ctrlId, PsxSingleController& cont) {
	struct AnalogDataCache {
		byte lx;
		byte ly;
		byte rx;
		byte ry;
	};

	static AnalogDataCache adCache[4];

	static PsxControllerProtocol protoCache[4] = {
		PSPROTO_UNKNOWN,
		PSPROTO_UNKNOWN,
		PSPROTO_UNKNOWN,
		PSPROTO_UNKNOWN
	};
	
	AnalogDataCache& cache = adCache[ctrlId];
	byte lx, ly, rx, ry;

	cont.getLeftAnalog (lx, ly);
	cont.getRightAnalog (rx, ry);
	if (cont.getButtonWord () != cont.getPreviousButtonWord () ||
	    (cont.analogSticksValid && (lx != cache.lx || ly != cache.ly ||
	     rx != cache.rx || ry != cache.ry)) ||
	     cont.protocol != protoCache[ctrlId]) {
			
		PsxButtons psxButtons = cont.getButtonWord ();
		
		Serial.print (F("Controller "));
		Serial.print ((char) ('A' + ctrlId));
		Serial.print (F(" ("));
		PGM_BYTES_P protoStr = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerProtocolStrings[cont.protocol < PSPROTO_MAX ? cont.protocol : (int) PSPROTO_MAX])));
		Serial.print (PSTR_TO_F (protoStr));
		Serial.print (F("): "));

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

		if (psxButtons != cont.getPreviousButtonWord () && cont.analogSticksValid) {
			Serial.print (", ");
		}

		if (cont.analogSticksValid) {
			Serial.print (F("Left Analog x = "));
			Serial.print (lx);
			Serial.print (F(", y = "));
			Serial.print (ly);
			Serial.print (", ");
			cache.lx = lx;
			cache.ly = ly;

			Serial.print (F("Right Analog x = "));
			Serial.print (rx);
			Serial.print (F(", y = "));
			Serial.print (ry);			
			cache.rx = rx;
			cache.ry = ry;
		}

		protoCache[ctrlId] = cont.protocol;
		
		Serial.println ();
	}
}


PsxDriverHwSpi<PIN_PS2_ATT> psxDriver;
//~ PsxDriverBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;
PsxMultiTap multitap;

boolean haveMultitap = false;
 
void setup () {
	Serial.begin (115200);
	
	//~ fastPinMode (PIN_BUTTONPRESS, OUTPUT);
	fastPinMode (PIN_HAVEMULTITAP, OUTPUT);
	
	delay (300);

	if (!psxDriver.begin ()) {
		Serial.println (F("Cannot initialize driver"));
		while (42)
			;
	}

	Serial.println (F("Ready!"));
}
 
void loop () {
	fastDigitalWrite (PIN_HAVEMULTITAP, haveMultitap);
	
	if (!haveMultitap) {
		if (multitap.begin (psxDriver)) {
			Serial.println (F("MultiTap found!"));
			delay (300);

			//~ if (!psx.enterConfigMode ()) {
				//~ Serial.println (F("Cannot enter config mode"));
			//~ } else {
				//~ PsxControllerType ctype = psx.getControllerType ();
				//~ PGM_BYTES_P cname = reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(controllerTypeStrings[ctype < PSCTRL_MAX ? static_cast<byte> (ctype) : PSCTRL_MAX])));
				//~ Serial.print (F("Controller Type is: "));
				//~ Serial.println (PSTR_TO_F (cname));

				//~ if (!psx.enableAnalogSticks ()) {
					//~ Serial.println (F("Cannot enable analog sticks"));
				//~ }
				
				//~ if (!psx.exitConfigMode ()) {
					//~ Serial.println (F("Cannot exit config mode"));
				//~ }
			//~ }
			
			haveMultitap = true;
		}
	} else {
		PsxSingleController *controllers;
		
		if (!multitap.readAll (&controllers)) {
			Serial.println (F("MultiTap lost :("));
			haveMultitap = false;
		} else {
			//~ fastDigitalWrite (PIN_BUTTONPRESS, !!psx.getButtonWord ());
			for (byte ctrlId = 0; ctrlId < 4; ++ctrlId) {
				PsxSingleController& cont = controllers[ctrlId];

				if (cont.protocol != PSPROTO_UNKNOWN) {
					dumpButtons (ctrlId, cont);
				} else {
					//~ Serial.print (F("Controller "));
					//~ Serial.print ((char) ('A' + ctrlId));
					//~ Serial.println (F(" not present"));
				}
			}
		}
	}
	
	delay (1000 / 60);
}
