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
 * This sketch was contributed by Kate (@katemonster33) and showcases Rumble
 * functionalities.
 */

#include <PsxControllerHwSpi.h>
#include <PsxNewLib.h>

PsxControllerHwSpi<10> psxCtrl;

boolean connected = false;
boolean axisConfigPossible = true;
boolean axisSticksEnabled = false;

void setup () {
	psxCtrl.begin ();
}

void loop () {
	if (connected && axisConfigPossible && !axisSticksEnabled) {
		Serial.println ("Attempting to enable axis sticks & rumble!");
		psxCtrl.setRumble (false, 0x00);

		if (psxCtrl.enterConfigMode() && psxCtrl.enableAnalogSticks (true, true)) {
			psxCtrl.enableRumble ();
			psxCtrl.exitConfigMode ();
			Serial.println ("Axis sticks enabled! Rumble enabled!");
			axisSticksEnabled = true;
		} else {
			Serial.println ("Failed to enable axis sticks!");
			axisConfigPossible = false; // don't retry
		}
	}

	if (psxCtrl.read ()) {
		if (!connected && Serial) {
			Serial.println ("Found controller!");
		}

		connected = true;

		if (axisSticksEnabled) {
			if (psxCtrl.buttonChanged (PSB_CROSS)) {
				if (psxCtrl.buttonPressed (PSB_CROSS)) {
					Serial.println ("Rumbling...");
					psxCtrl.setRumble (true, 0xFF);
				} else {
					Serial.println ("Ending rumble.");
					psxCtrl.setRumble (false, 0x00);
				}
			}
		}
	} else {
		if (connected && Serial) {
			Serial.println ("Lost controller!");
		}

		connected = false;
		axisSticksEnabled = false;
		axisConfigPossible = true;
	}
}
