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
 * This sketch was contributed by Matheus Fraguas (@sonik-br) and shows how the
 * library can be used to turn a PSX G-Con/GunCon controller into an USB mouse,
 * using an Arduino Leonardo. It is only compatible with the first version of
 * the gun, as subsequent versions connect to the console via USB.
 * 
 * It uses an edited version of AbsMouse Library. For details see:
 * https://github.com/jonathanedgecombe/absmouse
 * 
 * The guncon needs to "scan" the entire screen before it can properly send
 * the coordinates. Just point it at the screen and move slowly from side to
 * side and top to bottom. The values will be stored as min and max, and will be
 * used to calculate the absolute mouse position.
 * 
 * Buttons are mapped as follows:
 * - Trigger -> Circle -> Left mouse button
 * - A (Left side) -> Start -> Toggles gun to mouse movement
 * - B (Right side) -> Cross -> Right mouse button.
 */

#include <PsxControllerBitBang.h>
#include "AbsMouse.h"

/* We must use the bit-banging interface, as SPI pins are only available on the
 * ICSP header on the Leonardo.
*/
const byte PIN_PS2_ATT = 10;
const byte PIN_PS2_CMD = 11;
const byte PIN_PS2_DAT = 12;
const byte PIN_PS2_CLK = 13;

PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;

const byte PIN_BUTTONPRESS = A0;

const unsigned long POLLING_INTERVAL = 1000U / 50U;

boolean haveController = false;

// Minimum and maximum detected values. Varies from tv to tv.
word minX = -1;
word maxX = 0;
word minY = -1;
word maxY = 0;

// Last successful read coordinates
word lastX = -1;
word lastY = -1;

boolean enableMouseMove = true;

// Translate guncon values to the mouse absolute values (zero to 32767).
word convertRange (word gcMin, word gcMax, word value) {
	word scale = (word) (32767) / (gcMax - gcMin);
	return (word) ((value - gcMin) * scale);
}

void setup () {
	// Init AbsMouse library
	AbsMouse.init();

	Serial.begin (115200);

	Serial.println (F("Ready!"));
}

void loop () {
	static unsigned long last = 0;
	
	if (millis () - last >= POLLING_INTERVAL) {
		last = millis ();
		
		if (!haveController) {
			if (psx.begin ()) {
				Serial.println (F("Controller found!"));
			 
				haveController = true;
			}
		} else {
			if (!psx.read ()) {
				Serial.print (F("Controller lost, last values: x = "));
				Serial.print (lastX);
				Serial.print (F(", y = "));
				Serial.println (lastY);
				
				haveController = false;
			} else {
				// Read was successful, so let's make up data for Mouse

				// Handle trigger press/release, maps to left mouse button
				if (psx.buttonJustPressed (PSB_CIRCLE)) {
					Serial.println (F("Trigger press"));
					AbsMouse.press (MOUSE_LEFT);
				} else if (psx.buttonJustReleased (PSB_CIRCLE)) {
					Serial.println (F("Trigger release"));
					AbsMouse.release (MOUSE_LEFT);
				}
				
				// Handle btn A press/release, toggles gun to mouse movement
				if (psx.buttonJustPressed (PSB_START)) {
					Serial.println (F("Btn A press"));
					enableMouseMove = !enableMouseMove;
				} else if (psx.buttonJustReleased (PSB_START)) {
					Serial.println (F("Btn A release"));
				}
				
				// Handle btn B press/release, maps to right mouse button
				if (psx.buttonJustPressed (PSB_CROSS)) {
					Serial.println (F("Btn B press"));
					AbsMouse.press (MOUSE_RIGHT);
				} else if (psx.buttonJustReleased (PSB_CROSS)) {
					Serial.println (F("Btn B release"));
					AbsMouse.release (MOUSE_RIGHT);
				}

				// Get status and coordinates
				word x, y;
				GunconStatus gcStatus = psx.getGunconCoordinates (x, y);
				if (gcStatus == GUNCON_OK) {
					lastX = x;
					lastY = y;

					// Sets min and max detected values if needed
					if (x < minX && x > 70) {
						minX = x;
					} else if (x > maxX && x < 470) {
						maxX = x;
					}
						
					if (y < minY && y > 20) {
						minY = y;
					} else if (y > maxY && y < 300) {
						maxY = y;
					}

					Serial.print (F(" analog: x = "));
					Serial.print (x);
					Serial.print (F(", y = "));
					Serial.print (y);
	
					Serial.print (F(" MIN: x = "));
					Serial.print (minX);
					Serial.print (F(", y = "));
					Serial.print (minY);
	
					Serial.print (F(" MAX: x = "));
					Serial.print (maxX);
					Serial.print (F(", y = "));
					Serial.println (maxY);

					if (enableMouseMove) {
						AbsMouse.move (convertRange (minX, maxX, x),
						               convertRange (minY, maxY, y));
					}
				} else if (gcStatus == GUNCON_UNEXPECTED_LIGHT) {
					Serial.println (F("STATUS: GUNCON_UNEXPECTED_LIGHT!"));
				} else if (gcStatus == GUNCON_NO_LIGHT) {
					Serial.println (F("STATUS: GUNCON_NO_LIGHT!"));
				} else {
					Serial.println (F("STATUS: GUNCON_OTHER_ERROR!"));
				}
			}
		}
	}
}
