/*******************************************************************************
 * This file is part of PsxNewLib.                                             *
 *                                                                             *
 * Copyright (C) 2019-2021 by SukkoPera <software@sukkology.net>               *
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
 * This sketch was contributed by Eddie (@tylau0) and shows how the PS1 Densha
 * de Go! controllers can be converted to USB so that they can be used on
 * the PS4/Switch versions of Densha de Go!
 *
 * For more information, see
 * https://tylau0.blogspot.com/2020/12/using-ps1-densha-de-go-controller.html.

 * It uses ArduinoJoystickLibrary, for details see
 * https://github.com/MHeironimus/ArduinoJoystickLibrary.
 */
 
#include <PsxControllerBitBang.h>
#include <Joystick.h>

/* We must use the bit-banging interface, as SPI pins are only available on the
 * ICSP header on the Leonardo.
 *
 */
const byte PIN_PS2_ATT = 11;
const byte PIN_PS2_CMD = 9;
const byte PIN_PS2_DAT = 8;
const byte PIN_PS2_CLK = 10;

const unsigned long POLLING_INTERVAL = 1000U / 50U;

// Send debug messages to serial port
//#define ENABLE_SERIAL_DEBUG

PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;

Joystick_ usbStick (
	JOYSTICK_DEFAULT_REPORT_ID,
	JOYSTICK_TYPE_GAMEPAD, 
	24,     // buttonCount  // my change
	0,      // hatSwitchCount (0-2) // my change
	true,   // includeXAxis
	true,   // includeYAxis
	false,    // includeZAxis
	true,   // includeRxAxis
	true,   // includeRyAxis
	false,    // includeRzAxis
	false,    // includeRudder
	false,    // includeThrottle
	false,    // includeAccelerator
	false,    // includeBrake
	false   // includeSteering
);


#ifdef ENABLE_SERIAL_DEBUG
	#define dstart(spd) do {Serial.begin (spd); while (!Serial) {digitalWrite (LED_BUILTIN, (millis () / 500) % 2);}} while (0);
	#define debug(...) Serial.print (__VA_ARGS__)
	#define debugln(...) Serial.println (__VA_ARGS__)
#else
	#define dstart(...)
	#define debug(...)
	#define debugln(...)
#endif

boolean haveController = false;


#define toDegrees(rad) (rad * 180.0 / PI)

#define deadify(var, thres) (abs (var) > thres ? (var) : 0)


/** \brief Dead zone for analog sticks
 *
 * If the analog stick moves less than this value from the center position, it
 * is considered still.
 *
 * \sa ANALOG_IDLE_VALUE
 */
const byte ANALOG_DEAD_ZONE = 50U;


void setup () {
	// Lit the builtin led whenever buttons are pressed
	pinMode (LED_BUILTIN, OUTPUT);

	// Init Joystick library
	usbStick.begin (false);   // We'll call sendState() manually to minimize lag

	// This way we can output the same range of values we get from the PSX controller
	usbStick.setXAxisRange (ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
	usbStick.setYAxisRange (ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
	usbStick.setRxAxisRange (ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
	usbStick.setRyAxisRange (ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);

	dstart (115200);

	debugln (F("Ready!"));
}

void loop () {
	static unsigned long last = 0;
 
	if (millis () - last >= POLLING_INTERVAL) {
		last = millis ();
	 
		if (!haveController) {
			if (psx.begin ()) {
				debugln (F("Controller found!"));
				if (!psx.enterConfigMode ()) {
					debugln (F("Cannot enter config mode"));
				} else {
					// Try to enable analog sticks
					if (!psx.enableAnalogSticks ()) {
						debugln (F("Cannot enable analog sticks"));
					}
								 
					if (!psx.exitConfigMode ()) {
						debugln (F("Cannot exit config mode"));
					}
				}
			 
				haveController = true;
			}
		} else {
			if (!psx.read ()) {
				debugln (F("Controller lost :("));
				haveController = false;
			} else {
				// Buttons first!
				usbStick.setButton (0, psx.buttonPressed (PSB_SQUARE));
				usbStick.setButton (1, psx.buttonPressed (PSB_CROSS));
				usbStick.setButton (2, psx.buttonPressed (PSB_CIRCLE));
				usbStick.setButton (3, psx.buttonPressed (PSB_TRIANGLE));
				usbStick.setButton (4, psx.buttonPressed (PSB_L1));
				usbStick.setButton (5, psx.buttonPressed (PSB_R1));
				usbStick.setButton (13, psx.buttonPressed (PSB_L2));
				usbStick.setButton (14, psx.buttonPressed (PSB_R2));
				usbStick.setButton (8, psx.buttonPressed (PSB_SELECT));
				usbStick.setButton (9, psx.buttonPressed (PSB_START));
				usbStick.setButton (10, psx.buttonPressed (PSB_L3));
				usbStick.setButton (11, psx.buttonPressed (PSB_R3));
				usbStick.setButton (17, psx.buttonPressed (PSB_PAD_UP));
				usbStick.setButton (18, psx.buttonPressed (PSB_PAD_DOWN));
				usbStick.setButton (19, psx.buttonPressed (PSB_PAD_LEFT));
				usbStick.setButton (20, psx.buttonPressed (PSB_PAD_RIGHT));

				usbStick.setXAxis(ANALOG_IDLE_VALUE);
				usbStick.setYAxis(ANALOG_IDLE_VALUE);
				usbStick.setRxAxis(ANALOG_IDLE_VALUE);
				usbStick.setRyAxis(ANALOG_IDLE_VALUE);

				// All done, send data for real!
				usbStick.sendState ();
			}
		}
	}
} 
