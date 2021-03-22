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
 ******************************************************************************/
/**
 * \file PsxMultiTap.h
 * \author SukkoPera <software@sukkology.net>
 * \date 22 Mar 2021
 * \brief Playstation MultiTap interface library for Arduino
 * 
 * Please refer to the GitHub page and wiki for any information:
 * https://github.com/SukkoPera/PsxNewLib
 */

#ifndef PSXMULTITAP_H_
#define PSXMULTITAP_H_

#include "PsxDriver.h"
#include "PsxPublicTypes.h"
#include "PsxOptions.h"
#include "PsxCommands.h"


/** \brief PSX Controller Interface
 * 
 * This is the base class implementing interactions with PSX controllers. It is
 * partially abstract, so it is not supposed to be instantiated directly.
 */
template <uint8_t N_CONTROLLERS>
class PsxMultiTapTemplate {
protected:
	PsxDriver *driver;

	PsxSingleController controllers[N_CONTROLLERS];

	// Green Mode controllers
	inline boolean isFlightstickReply (const byte *status) {
		return (status[1] & 0xF0) == 0x50;
	}

	inline boolean isDualShockReply (const byte *status) {
		return (status[1] & 0xF0) == 0x70;
	}

	inline boolean isDualShock2Reply (const byte *status) {
		return status[1] == 0x79;
	}

	inline boolean isDigitalReply (const byte *status) {
		return (status[1] & 0xF0) == 0x40;
	}

	inline boolean isConfigReply (const byte *status) {
		return (status[1] & 0xF0) == 0xF0;
	}

	inline boolean isNegconReply (const byte *status) {
		return status[1] == 0x23;
	}

	inline boolean isJogconReply (const byte *status) {
		return (status[1] & 0xF0) == 0xE0;
	}

	inline boolean isMultiTapReply (const byte *status) {
		return (status[1] & 0xF0) == 0x80;
	}

	boolean haveMultiTap () {
		/* This will enable the MultiTap, if present, but still return data as
		* a normal read. Actual MultiTap data will be returned <i>at the next
		* read</i>.
		*/
		driver -> attention ();
		driver -> autoShift (multipoll, sizeof (multipoll));
		driver -> noAttention ();

		// Do not rush :)
		delay (16);

		/* This will return MultiTap data, if present, and cause the next read
		* to return normal data again
		*/
		driver -> attention ();
		byte *in = driver -> autoShift (multipoll, sizeof (multipoll));
		driver -> noAttention ();

		return in != NULL && isMultiTapReply (in);
	}

public:
	/** \brief Initialize library
	 * 
	 * This function shall be called before any others, it will initialize the
	 * communication and return if a supported controller was found. It shall
	 * also be called to reinitialize the communication whenever the controller
	 * is unplugged.
	 * 
	 * Derived classes can override this function if they need to perform
	 * additional initializations, but shall call it on return.
	 * 
	 * \return true if a supported controller was found, false otherwise
	 */
	virtual boolean begin (PsxDriver& drv) {
		driver = &drv;
		
		//~ for (byte i = 0; i < N_CONTROLLERS; ++i) {
			//~ controllers[i].clear ();
		//~ }

		return haveMultiTap ();
	}

	//! \name Configuration Mode Functions
	//! @{
	
	/** \brief Enter Configuration Mode
	 * 
	 * Some controllers can be configured in several aspects. For instance,
	 * DualShock controllers can return analog stick data. This function puts
	 * the controller in configuration mode.
	 * 
	 * Note that <i>Configuration Mode</i> is sometimes called <i>Escape Mode</i>.
	 * 
	 * \return true if Configuration Mode was entered successfully
	 */
	//~ boolean enterConfigMode () {
		//~ boolean ret = false;

		//~ unsigned long start = millis ();
		//~ do {
			//~ driver -> attention ();
			//~ byte *in = driver -> autoShift (enter_config, 4);
			//~ driver -> noAttention ();

			//~ ret = in != NULL && isConfigReply (in);

			//~ if (!ret) {
				//~ delay (COMMAND_RETRY_INTERVAL);
			//~ }
		//~ } while (!ret && millis () - start <= COMMAND_TIMEOUT);
		//~ delay (MODE_SWITCH_DELAY);

		//~ return ret;
	//~ }

	/** \brief Enable (or disable) analog sticks
	 * 
	 * This function enables or disables the analog sticks that were introduced
	 * with DualShock controllers. When they are enabled, the getLeftAnalog()
	 * and getRightAnalog() functions can be used to retrieve their positions.
	 * Also, button presses for L3 and R3 will be available through the
	 * buttonPressed() and similar functions.
	 * 
	 * When analog sticks are enabled, the \a ANALOG led will light up (in red)
	 * on the controller.
	 * 
	 * Note that on some third-party controllers, when analog sticks are
	 * disabled the analog levers will "emulate" the D-Pad and possibly the
	 * []/^/O/X buttons. This does not happen on official Sony controllers.
	 * 
	 * This function will only work if when the controller is in Configuration
	 * Mode.
	 * 
	 * \param[in] enabled true to enable, false to disable
	 * \param[in] locked If true, the \a ANALOG button on the controller will be
	 *                   disabled and the user will not be able to turn off the
	 *                   analog sticks.
	 * \return true if the command was ackowledged by the controller. Note that
	 *         this does not fully guarantee that the analog sticks were enabled
	 *         as this can only be checked after Configuration Mode is exited.
	 */
	//~ boolean enableAnalogSticks (bool enabled = true, bool locked = false) {
		//~ boolean ret = false;
		//~ byte out[sizeof (set_mode)];

		//~ memcpy (out, set_mode, sizeof (set_mode));
		//~ out[3] = enabled ? 0x01 : 0x00;
		//~ out[4] = locked ? 0x03 : 0x00;

		//~ unsigned long start = millis ();
		//~ byte cnt = 0;
		//~ do {
			//~ driver -> attention ();
			//~ byte *in = driver -> autoShift (out, 5);
			//~ driver -> noAttention ();

			//~ /* We can't know if we have successfully enabled analog mode until
			 //~ * we get out of config mode, so let's just be happy if we get a few
			 //~ * consecutive valid replies
			 //~ */
			//~ if (in != nullptr) {
				//~ ++cnt;
			//~ }
			//~ ret = cnt >= 3;

			//~ if (!ret) {
				//~ delay (COMMAND_RETRY_INTERVAL);
			//~ }
		//~ } while (!ret && millis () - start <= COMMAND_TIMEOUT);
		//~ delay (MODE_SWITCH_DELAY);

		//~ return ret;
	//~ }

	/** \brief Enable (or disable) analog buttons
	 * 
	 * This function enables or disables the analog buttons that were introduced
	 * with DualShock 2 controllers. When they are enabled, the
	 * getAnalogButton() functions can be used to retrieve how deep/strongly
	 * they are pressed. This applies to the D-Pad buttons, []/^/O/X, L1/2 and
	 * R1/2
	 * 
	 * This function will only work if when the controller is in Configuration
	 * Mode.
	 * 
	 * \param[in] enabled true to enable, false to disable
	 * \return true if the command was ackowledged by the controller. Note that
	 *         this does not fully guarantee that the analog sticks were enabled
	 *         as this can only be checked after Configuration Mode is exited.
	 */
	//~ boolean enableAnalogButtons (bool enabled = true) {
		//~ boolean ret = false;
		//~ byte out[sizeof (set_mode)];

		//~ memcpy (out, set_pressures, sizeof (set_pressures));
		//~ if (!enabled) {
			//~ out[3] = 0x00;
			//~ out[4] = 0x00;
			//~ out[5] = 0x00;
		//~ }

		//~ unsigned long start = millis ();
		//~ byte cnt = 0;
		//~ do {
			//~ driver -> attention ();
			//~ byte *in = driver -> autoShift (out, sizeof (set_pressures));
			//~ driver -> noAttention ();

			//~ /* We can't know if we have successfully enabled analog mode until
			 //~ * we get out of config mode, so let's just be happy if we get a few
			 //~ * consecutive valid replies
			 //~ */
			//~ if (in != nullptr) {
				//~ ++cnt;
			//~ }
			//~ ret = cnt >= 3;

			//~ if (!ret) {
				//~ delay (COMMAND_RETRY_INTERVAL);
			//~ }
		//~ } while (!ret && millis () - start <= COMMAND_TIMEOUT);
		//~ delay (MODE_SWITCH_DELAY);

		//~ return ret;
	//~ }

	/** \brief Retrieve the controller type
	 * 
	 * This function retrieves the controller type. It is not 100% reliable, so
	 * do not rely on it for anything other than a vague indication (for
	 * instance, the DualShock SCPH-1200 controller gets reported as the Guitar
	 * Hero controller...).
	 * 
	 * This function will only work if when the controller is in Configuration
	 * Mode.
	 * 
	 * \return The (tentative) controller type
	 */
	//~ PsxControllerType getControllerType () {
		//~ PsxControllerType ret = PSCTRL_UNKNOWN;

		//~ driver -> attention ();
		//~ byte *in = driver -> autoShift (type_read, 3);
		//~ driver -> noAttention ();

		//~ if (in != nullptr) {
			//~ const byte& controllerType = in[3];
			//~ if (controllerType == 0x03) {
				//~ ret = PSCTRL_DUALSHOCK;
			//~ // } else if (controllerType == 0x01 && in[1] == 0x42) {
				//~ // return 4;		// ???
			//~ }  else if (controllerType == 0x01 && in[1] != 0x42) {
				//~ ret = PSCTRL_GUITHERO;
			//~ } else if (controllerType == 0x0C) {
				//~ ret = PSCTRL_DSWIRELESS;
			//~ }
		//~ }

		//~ return ret;
	//~ }

	//~ boolean exitConfigMode () {
		//~ boolean ret = false;

		//~ unsigned long start = millis ();
		//~ do {
			//~ driver -> attention ();
			//~ // shiftInOut (poll, in, sizeof (poll));
			//~ // shiftInOut (exit_config, in, sizeof (exit_config));
			//~ byte *in = driver -> autoShift (exit_config, 4);
			//~ driver -> noAttention ();

			//~ ret = in != nullptr && !isConfigReply (in);

			//~ if (!ret) {
				//~ delay (COMMAND_RETRY_INTERVAL);
			//~ }
		//~ } while (!ret && millis () - start <= COMMAND_TIMEOUT);
		//~ delay (MODE_SWITCH_DELAY);

		//~ return ret;
	//~ }

	//! @}		// Configuration Mode Functions
	
	//! \name Polling Functions
	//! @{

	/** \brief Retrieve the controller protocol
	 * 
	 * This function retrieves the protocol that was used to interpret
	 * controller data at the last call to read().
	 * 
	 * \return The controller protocol
	 */
	//~ PsxControllerProtocol getProtocol () const {
		//~ return protocol;
	//~ }

	/** \brief Poll the controller
	 * 
	 * This function polls the controller for button and stick data. It self-
	 * adapts to all the supported controller types and populates internal
	 * variables with the retrieved information, which can be later accessed
	 * through the inspection functions.
	 * 
	 * This function must be called quite often in order to keep the controller
	 * alive. Most controllers have some kind of watchdog that will reset them
	 * if they don't get polled at least every so often (like a couple dozen
	 * times per seconds).
	 * 
	 * If this function fails repeatedly, it can safely be assumed that the
	 * controller has been disconnected (or that it is not supported if it
	 * failed right from the beginning).
	 * 
	 * \return true if the read was successful, false otherwise
	 */
	//~ boolean read () {
		//~ boolean ret = false;

		//~ analogSticksValid = false;
		//~ analogButtonDataValid = false;

		//~ driver -> attention ();
		//~ byte *in = driver -> autoShift (poll, 3);
		//~ driver -> noAttention ();

		//~ if (in != NULL) {
			//~ if (isConfigReply (in)) {
				//~ // We're stuck in config mode, try to get out
				//~ exitConfigMode ();
			//~ } else {
				//~ // We surely have buttons
				//~ previousButtonWord = buttonWord;
				//~ buttonWord = ((PsxButtons) in[4] << 8) | in[3];

				//~ // See if we have anything more to read
				//~ if (isDualShock2Reply (in)) {
					//~ protocol = PSPROTO_DUALSHOCK2;
				//~ } else if (isDualShockReply (in)) {
					//~ protocol = PSPROTO_DUALSHOCK;
				//~ } else if (isFlightstickReply (in)) {
					//~ protocol = PSPROTO_FLIGHTSTICK;
				//~ } else if (isNegconReply (in)) {
					//~ protocol = PSPROTO_NEGCON;
				//~ } else if (isJogconReply (in)) {
					//~ protocol = PSPROTO_JOGCON;
				//~ } else {
					//~ protocol = PSPROTO_DIGITAL;
				//~ }

				//~ switch (protocol) {
					//~ case PSPROTO_DUALSHOCK2:
						//~ // We also have analog button data
						//~ analogButtonDataValid = true;
						//~ for (int i = 0; i < PSX_ANALOG_BTN_DATA_SIZE; ++i) {
							//~ analogButtonData[i] = in[i + 9];
						//~ }
						//~ /* Now fall through to DualShock case, the next line
						 //~ * avoids GCC warning
						 //~ */
						//~ /* FALLTHRU */
					//~ case PSPROTO_DUALSHOCK:
					//~ case PSPROTO_FLIGHTSTICK:
						//~ // We have analog stick data
						//~ analogSticksValid = true;
						//~ rx = in[5];
						//~ ry = in[6];
						//~ lx = in[7];
						//~ ly = in[8];
						//~ break;
					//~ case PSPROTO_NEGCON:
						//~ // Map the twist axis to X axis of left analog
						//~ analogSticksValid = true;
						//~ lx = in[5];

						//~ // Map analog button data to their reasonable counterparts
						//~ analogButtonDataValid = true;
						//~ analogButtonData[PSAB_CROSS] = in[6];
						//~ analogButtonData[PSAB_SQUARE] = in[7];
						//~ analogButtonData[PSAB_L1] = in[8];

						//~ // Make up "missing" digital data
						//~ if (analogButtonData[PSAB_SQUARE] >= NEGCON_I_II_BUTTON_THRESHOLD) {
							//~ buttonWord &= ~PSB_SQUARE;
						//~ }
						//~ if (analogButtonData[PSAB_CROSS] >= NEGCON_I_II_BUTTON_THRESHOLD) {
							//~ buttonWord &= ~PSB_CROSS;
						//~ }
						//~ if (analogButtonData[PSAB_L1] >= NEGCON_L_BUTTON_THRESHOLD) {
							//~ buttonWord &= ~PSB_L1;
						//~ }
						//~ break;
					//~ case PSPROTO_JOGCON:
						//~ /* Map the wheel X axis of left analog, half a rotation
						 //~ * per direction: byte 5 has the wheel position, it is
						 //~ * 0 at startup, then we have 0xFF down to 0x80 for
						 //~ * left/CCW, and 0x01 up to 0x80 for right/CW
						 //~ *
						 //~ * byte 6 is the number of full CW rotations
						 //~ * byte 7 is 0 if wheel is still, 1 if it is rotating CW
						 //~ *        and 2 if rotation CCW
						 //~ * byte 8 seems to stay at 0
						 //~ *
						 //~ * We'll want to cap the movement halfway in each
						 //~ * direction, for ease of use/implementation.
						 //~ */
						//~ analogSticksValid = true;
						//~ if (in[6] < 0x80) {
							//~ // CW up to half
							//~ lx = in[5] < 0x80 ? in[5] : (0x80 - 1);
						//~ } else {
							//~ // CCW down to half
							//~ lx = in[5] > 0x80 ? in[5] : (0x80 + 1);
						//~ }

						//~ // Bring to the usual 0-255 range
						//~ lx += 0x80;
						//~ break;
					//~ default:
						//~ // We are already done
						//~ break;
				//~ }
				
				//~ ret = true;
			//~ }
		//~ }

		//~ return ret;
	//~ }
	
	boolean readAll (PsxSingleController **outControllers) {
		boolean ret = false;

		byte out[35] = {};
		memcpy (out, multipoll, sizeof (multipoll));
		out[3] = 0x42;
		out[11] = 0x42;
		out[19] = 0x42;
		out[27] = 0x42;
		
		driver -> attention ();
		byte *in = driver -> autoShift (out, sizeof (out));
		driver -> noAttention ();

		if (in != NULL) {			
			if (isMultiTapReply (in)) {
				//~ for (byte i = 0; i < driver -> getReplyLength (in); ++i) {
					//~ Serial.print (in[i], HEX);
					//~ Serial.print (' ');
				//~ }
				//~ Serial.println ();
				
				byte *ptr = in + 2;
				for (byte i = 0; i < N_CONTROLLERS; ++i) {
					PsxSingleController& cont = controllers[i];
					
					cont.analogSticksValid = false;
					cont.analogButtonDataValid = false;

					cont.previousButtonWord = cont.buttonWord;
					cont.buttonWord = ((PsxButtons) ptr[4] << 8) | ptr[3];

					// See if we have anything more to read
					// I guess the PS1 Multitap is not compatible with DS2 controllers...
					/*if (isDualShock2Reply (ptr)) {
						cont.protocol = PSPROTO_DUALSHOCK2;
					} else*/ if (isDualShockReply (ptr)) {
						cont.protocol = PSPROTO_DUALSHOCK;
					} else if (isFlightstickReply (ptr)) {
						cont.protocol = PSPROTO_FLIGHTSTICK;
					} else if (isNegconReply (ptr)) {
						cont.protocol = PSPROTO_NEGCON;
					} else if (isJogconReply (ptr)) {
						cont.protocol = PSPROTO_JOGCON;
					} else if (isDigitalReply (ptr)) {
						cont.protocol = PSPROTO_DIGITAL;
					} else {
						cont.protocol = PSPROTO_UNKNOWN;
					}

					switch (cont.protocol) {
						//~ case PSPROTO_DUALSHOCK2:
							//~ // We also have analog button data
							//~ cont.analogButtonDataValid = true;
							//~ for (int i = 0; i < PSX_ANALOG_BTN_DATA_SIZE; ++i) {
								//~ cont.analogButtonData[i] = in[i + 9];
							//~ }
							//~ /* Now fall through to DualShock case, the next line
							 //~ * avoids GCC warning
							 //~ */
							//~ /* FALLTHRU */
						case PSPROTO_DUALSHOCK:
						case PSPROTO_FLIGHTSTICK:
							// We have analog stick data
							cont.analogSticksValid = true;
							cont.rx = ptr[5];
							cont.ry = ptr[6];
							cont.lx = ptr[7];
							cont.ly = ptr[8];
							break;
						case PSPROTO_NEGCON:
							// Map the twist axis to X axis of left analog
							cont.analogSticksValid = true;
							cont.lx = ptr[5];

							// Map analog button data to their reasonable counterparts
							cont.analogButtonDataValid = true;
							cont.analogButtonData[PSAB_CROSS] = ptr[6];
							cont.analogButtonData[PSAB_SQUARE] = ptr[7];
							cont.analogButtonData[PSAB_L1] = ptr[8];

							// Make up "missing" digital data
							if (cont.analogButtonData[PSAB_SQUARE] >= NEGCON_I_II_BUTTON_THRESHOLD) {
								cont.buttonWord &= ~PSB_SQUARE;
							}
							if (cont.analogButtonData[PSAB_CROSS] >= NEGCON_I_II_BUTTON_THRESHOLD) {
								cont.buttonWord &= ~PSB_CROSS;
							}
							if (cont.analogButtonData[PSAB_L1] >= NEGCON_L_BUTTON_THRESHOLD) {
								cont.buttonWord &= ~PSB_L1;
							}
							break;
						case PSPROTO_JOGCON:
							/* Map the wheel X axis of left analog, half a rotation
							 * per direction: byte 5 has the wheel position, it is
							 * 0 at startup, then we have 0xFF down to 0x80 for
							 * left/CCW, and 0x01 up to 0x80 for right/CW
							 *
							 * byte 6 is the number of full CW rotations
							 * byte 7 is 0 if wheel is still, 1 if it is rotating CW
							 *        and 2 if rotation CCW
							 * byte 8 seems to stay at 0
							 *
							 * We'll want to cap the movement halfway in each
							 * direction, for ease of use/implementation.
							 */
							cont.analogSticksValid = true;
							if (ptr[6] < 0x80) {
								// CW up to half
								cont.lx = ptr[5] < 0x80 ? ptr[5] : (0x80 - 1);
							} else {
								// CCW down to half
								cont.lx = ptr[5] > 0x80 ? ptr[5] : (0x80 + 1);
							}

							// Bring to the usual 0-255 range
							cont.lx += 0x80;
							break;
						default:
							// We are already done
							break;
					}

					ptr += 8;
				}

				*outControllers = controllers;
				ret = true;
			}
		}

		return ret;
	}
	
	//! @}		// Polling Functions
};

typedef PsxMultiTapTemplate<4> PsxMultiTap;

#endif
