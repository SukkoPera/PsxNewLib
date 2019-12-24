/*******************************************************************************
 * This file is part of PsxNewLib.                                             *
 *                                                                             *
 * Copyright (C) 2019 by SukkoPera <software@sukkology.net>                    *
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
 * \file PsxNewLib.h
 * \author SukkoPera <software@sukkology.net>
 * \date 16 Dec 2019
 * \brief Playstation controller interface library for Arduino
 * 
 * Please refer to the GitHub page and wiki for any information:
 * https://github.com/SukkoPera/PsxNewLib
 */

#ifndef PSXNEWLIB_H_
#define PSXNEWLIB_H_

// Uncomment this to have all byte exchanges logged to serial
//~ #define DUMP_COMMS

/** \brief Command Inter-Byte Delay (us)
 * 
 * Commands are several bytes long. This is the time to wait between two
 * consecutive bytes.
 * 
 * This should actually be done by watching the \a Acknowledge line, but we are
 * ignoring it at the moment.
 */
const byte INTER_CMD_BYTE_DELAY = 15;

/** \brief Command timeout (ms)
 * 
 * Commands are sent to the controller repeatedly, until they succeed or time
 * out. This is the length of that timeout.
 * 
 * \sa COMMAND_RETRY_INTERVAL
 */
const unsigned long COMMAND_TIMEOUT = 250;

/** \brief Command Retry Interval (ms)
 * 
 * When sending a command to the controller, if it does not succeed, it is
 * retried after this amount of time.
 */
const unsigned long COMMAND_RETRY_INTERVAL = 10;

/** \brief Mode switch delay (ms)
 * 
 * After a command has been issued successfully to the controller, this amount
 * of time is waited to allow it to complete any internal procedures required to
 * execute the command.
 * 
 * \todo This is probably unnecessary.
 */
const unsigned long MODE_SWITCH_DELAY = 500;


/** \brief Type that is used to represent a single button in most places
 */
enum PsxButton {
	PSB_NONE       = 0x0000,
	PSB_SELECT     = 0x0001,
	PSB_L3         = 0x0002,
	PSB_R3         = 0x0004,
	PSB_START      = 0x0008,
	PSB_PAD_UP     = 0x0010,
	PSB_PAD_RIGHT  = 0x0020,
	PSB_PAD_DOWN   = 0x0040,
	PSB_PAD_LEFT   = 0x0080,
	PSB_L2         = 0x0100,
	PSB_R2         = 0x0200,
	PSB_L1         = 0x0400,
	PSB_R1         = 0x0800,
	PSB_GREEN      = 0x1000,
	PSB_RED        = 0x2000,
	PSB_BLUE       = 0x4000,
	PSB_PINK       = 0x8000,
	PSB_TRIANGLE   = 0x1000,
	PSB_CIRCLE     = 0x2000,
	PSB_CROSS      = 0x4000,
	PSB_SQUARE     = 0x8000
};

/** \brief Type that is used to represent a single button when retrieving
 *         analog pressure data
 *
 * \sa getAnalogButton()
 */
enum PsxAnalogButton {
	PSAB_PAD_RIGHT  = 0,
	PSAB_PAD_LEFT   = 1,
	PSAB_PAD_UP     = 2,
	PSAB_PAD_DOWN   = 3,
	PSAB_TRIANGLE   = 4,
	PSAB_CIRCLE     = 5,
	PSAB_CROSS      = 6,
	PSAB_SQUARE     = 7,
	PSAB_L1         = 8,
	PSAB_R1         = 9,
	PSAB_L2         = 10,
	PSAB_R2         = 11
};

/** \brief Type that is used to report button presses
 */
typedef uint16_t PsxButtons;

//! \name Controller Commands
//! @{
/** \brief Enter Configuration Mode
 * 
 * Command used to enter the controller configuration (also known as \a escape)
 * mode
 */
static byte enter_config[] = {0x01, 0x43, 0x00, 0x01, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
static byte exit_config[] = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
/* These shorter versions of enter_ and exit_config are accepted by all
 * controllers I've tested, even in analog mode, EXCEPT SCPH-1200, so let's use
 * the longer ones
 */
//~ static byte enter_config[] = {0x01, 0x43, 0x00, 0x01, 0x00};
//~ static byte exit_config[] = {0x01, 0x43, 0x00, 0x00, 0x00};

/** \brief Read Controller Type
 * 
 * Command used to read the controller type.
 * 
 * This does not seem to be 100% reliable, or at least we don't know how to tell
 * all the various controllers apart.
 */
static byte type_read[] = {0x01, 0x45, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
static byte set_mode[] = {0x01, 0x44, 0x00, /* enabled */ 0x01, /* locked */ 0x03, 0x00, 0x00, 0x00, 0x00};
static byte set_pressures[] = {0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
//~ static byte enable_rumble[] = {0x01, 0x4D, 0x00, 0x00, 0x01};

/** \brief Poll all buttons
 * 
 * Command used to read the status of all buttons.
 */
static byte poll[] = {0x01, 0x42, 0x00, 0xFF, 0xFF};
//! @}

enum PsxControllerType {
	PSCTRL_UNKNOWN = 0,
	PSCTRL_DUALSHOCK,
	PSCTRL_DSWIRELESS,
	PSCTRL_GUITHERO
};

const byte PSCTRL_MAX = static_cast<byte> (PSCTRL_GUITHERO) + 1;

/** \brief PSX Controller Interface
 * 
 * This is the base class implementing interactions with PSX controllers. It is
 * partially abstract, so it is not supposed to be instantiated directly.
 */
class PsxController {
protected:
	/** \brief Size of internal communication buffer
	 * 
	 * This can be sized after the longest command reply (which is 21 bytes for
	 * 01 42 when in DualShock 2 mode), but we're better safe than sorry.
	 */
	static const byte BUFFER_SIZE = 32;

	/** \brief Size of buffer holding analog button data
	 */
	static const byte ANALOG_BTN_DATA_SIZE = 12;

	/** \brief Internal communication buffer
	 * 
	 * This is used to hold replies received from the controller.
	 */
	byte inputBuffer[BUFFER_SIZE];

	/** \brief Previous (Digital) Button status
	 * 
	 * The individual bits can be identified through #PsxButton.
	 */
	PsxButtons previousButtonWord;

	/** \brief (Digital) Button status
	 * 
	 * The individual bits can be identified through #PsxButton.
	 */
	PsxButtons buttonWord;

	//! \name Analog Stick Data
	//! @{
	byte lx;		//!< Horizontal axis of left stick [0-255, L to R]
	byte ly;		//!< Vertical axis of left stick [0-255, U to D]
	byte rx;		//!< Horizontal axis of right stick [0-255, L to R]
	byte ry;		//!< Vertical axis of right stick [0-255, U to D]
	
	boolean analogSticksValid;	//!< True if the above were valid in last call to read()
	//! @}
	
	/** \brief Analog Button Data
	 * 
	 * \todo What's the meaning of every individual byte?
	 */
	byte analogButtonData[ANALOG_BTN_DATA_SIZE];

	/** \brief Analog Button Data Validity
	 * 
	 * True if the #analogButtonData were valid in last call to read()
	 */
	boolean analogButtonDataValid;

	/** \brief Assert the Attention line
	 * 
	 * This function must be implemented by derived classes and must set the
	 * Attention line \a low so that the controller will pay attention to what
	 * we will send.
	 */
	virtual void attention () = 0;

	/** \brief Deassert the Attention line
	 * 
	 * This function must be implemented by derived classes and must set the
	 * Attention line \a high so that the controller will no longer pay
	 * attention to what we will send.
	 */
	virtual void noAttention () = 0;

	/** \brief Transfer a single byte to/from the controller
	 * 
	 * This function must be implemented by derived classes and must transfer
	 * a single <i>command</i> byte to the controller and read back a single
	 * <i>data</i> byte.
	 * 
	 * \param[in] out The command byte to send the controller
	 * \return The data byte returned by the controller
	 */
	virtual byte shiftInOut (const byte out) = 0;

	/** \brief Transfer several bytes to/from the controller
	 * 
	 * This function transfers an array of <i>command</i> bytes to the
	 * controller and reads back an equally sized array of <i>data</i> bytes.
	 * 
	 * \param[in] out The command bytes to send the controller
	 * \param[out] in The data bytes returned by the controller, must be sized
	 *                 to hold at least \a len bytes
	 * \param[in] len The amount of bytes to be exchanged
	 */
	void shiftInOut (const byte *out, byte *in, const byte len) {
#ifdef DUMP_COMMS
		byte inbuf[len];
#endif

		for (byte i = 0; i < len; ++i) {
			byte tmp = shiftInOut (out != NULL ? out[i] : 0x5A);
#ifdef DUMP_COMMS
			inbuf[i] = tmp;
#endif
			if (in != NULL) {
				in[i] = tmp;
			}

			delayMicroseconds (INTER_CMD_BYTE_DELAY);   // Very important!
		}

#ifdef DUMP_COMMS
		Serial.print (F("<-- "));
		for (byte i = 0; i < len; ++i) {
			if (out && out[i] < 0x10)
				Serial.print (0);
			Serial.print (out ? out[i]: 0x5A, HEX);
			Serial.print (' ');
		}
		Serial.println ();

		Serial.print (F("--> "));
		for (byte i = 0; i < len; ++i) {
			if (inbuf[i] < 0x10)
				Serial.print (0);
			Serial.print (inbuf[i], HEX);
			Serial.print (' ');
		}
		Serial.println ();
#endif
	}

	/** \brief Transfer several bytes to/from the controller
	 * 
	 * This function transfers an array of <i>command</i> bytes to the
	 * controller and reads back the full reply of <i>data</i> bytes. The size
	 * of the reply is calculated automatically and padding bytes (0x5A) are
	 * appended to the outgoing message if it is shorter.
	 * 
	 * The reply is stored in an internal buffer and will be valid until the
	 * next call to this function, so make sure to save anything if is needed.
	 * 
	 * \param[out] out The data bytes returned by the controller, must be sized
	 *                 to hold at least \a len bytes
	 * \param[in] len The amount of bytes to be exchanged
	 * \return A pointer to a buffer containing the reply, whose size can be
	 *         calculated with getReplyLength()
	 */
	byte *autoShift (const byte *out, const byte len) {
		byte *ret = nullptr;

		if (len >= 3 && len <= BUFFER_SIZE) {
			// All commands have at least 3 bytes, so shift out those first
			shiftInOut (out, inputBuffer, 3);
			if (isValidReply (inputBuffer)) {
				// Reply is good, get full length
				byte replyLen = getReplyLength (inputBuffer);

				// Shift out rest of command
				if (len > 3) {
					shiftInOut (out + 3, inputBuffer + 3, len - 3);
				}

				byte left = replyLen - len + 3;
				//~ Serial.print ("len = ");
				//~ Serial.print (replyLen);
				//~ Serial.print (", left = ");
				//~ Serial.println (left);
				if (left == 0) {
					// The whole reply was gathered
					ret = inputBuffer;
				} else if (len + left <= BUFFER_SIZE) {
					// Part of reply is still missing and we have space for it
					shiftInOut (NULL, inputBuffer + len, left);
					ret = inputBuffer;
				} else {
					// Reply incomplete but not enough space provided
				}
			}
		}

		return ret;
	}

	/** \brief Get reply length
	 * 
	 * Calculates the length of a command reply, in bytes
	 * 
	 * \param[in] buf The buffer containing the reply, must be at least 2 bytes
	 *                long
	 * \return The calculated length
	 */
	byte getReplyLength (const byte *buf) const {
		return (buf[1] & 0x0F) * 2;
	}

	inline boolean isValidReply (const byte *status) {
		//~ return status[0] != 0xFF || status[1] != 0xFF || status[2] != 0xFF;
		return status[1] != 0xFF && (status[2] == 0x5A || status[2] == 0x00);
		//~ return /* status[0] == 0xFF && */ status[1] != 0xFF && status[2] == 0x5A;
	}

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
	virtual boolean begin () {
		lx = 0;
		ly = 0;
		rx = 0;
		ry = 0;

		analogSticksValid = false;

		// Some disposable readings to let the controller know we are here
		for (byte i = 0; i < 5; ++i) {
			read ();
			delay (1);
		}

		return read ();
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
	boolean enterConfigMode () {
		boolean ret = false;

		unsigned long start = millis ();
		do {
			attention ();
			byte *in = autoShift (enter_config, 4);
			noAttention ();

			ret = in != NULL && isConfigReply (in);

			if (!ret) {
				delay (COMMAND_RETRY_INTERVAL);
			}
		} while (!ret && millis () - start <= COMMAND_TIMEOUT);
		delay (MODE_SWITCH_DELAY);

		return ret;
	}

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
	boolean enableAnalogSticks (bool enabled = true, bool locked = false) {
		boolean ret = false;
		byte out[sizeof (set_mode)];

		memcpy (out, set_mode, sizeof (set_mode));
		out[3] = enabled ? 0x01 : 0x00;
		out[4] = locked ? 0x03 : 0x00;

		unsigned long start = millis ();
		byte cnt = 0;
		do {
			attention ();
			byte *in = autoShift (out, 5);
			noAttention ();

			/* We can't know if we have successfully enabled analog mode until
			 * we get out of config mode, so let's just be happy if we get a few
			 * consecutive valid replies
			 */
			if (in != nullptr) {
				++cnt;
			}
			ret = cnt >= 3;

			if (!ret) {
				delay (COMMAND_RETRY_INTERVAL);
			}
		} while (!ret && millis () - start <= COMMAND_TIMEOUT);
		delay (MODE_SWITCH_DELAY);

		return ret;
	}

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
	boolean enableAnalogButtons (bool enabled = true) {
		boolean ret = false;
		byte out[sizeof (set_mode)];

		memcpy (out, set_pressures, sizeof (set_pressures));
		if (!enabled) {
			out[3] = 0x00;
			out[4] = 0x00;
			out[5] = 0x00;
		}

		unsigned long start = millis ();
		byte cnt = 0;
		do {
			attention ();
			byte *in = autoShift (out, sizeof (set_pressures));
			noAttention ();

			/* We can't know if we have successfully enabled analog mode until
			 * we get out of config mode, so let's just be happy if we get a few
			 * consecutive valid replies
			 */
			if (in != nullptr) {
				++cnt;
			}
			ret = cnt >= 3;

			if (!ret) {
				delay (COMMAND_RETRY_INTERVAL);
			}
		} while (!ret && millis () - start <= COMMAND_TIMEOUT);
		delay (MODE_SWITCH_DELAY);

		return ret;
	}

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
	PsxControllerType getControllerType () {
		PsxControllerType ret = PSCTRL_UNKNOWN;

		attention ();
		byte *in = autoShift (type_read, 3);
		noAttention ();

		if (in != nullptr) {
			const byte& controllerType = in[3];
			if (controllerType == 0x03) {
				ret = PSCTRL_DUALSHOCK;
			//~ } else if (controllerType == 0x01 && in[1] == 0x42) {
				//~ return 4;		// ???
			}  else if (controllerType == 0x01 && in[1] != 0x42) {
				ret = PSCTRL_GUITHERO;
			} else if (controllerType == 0x0C) {
				ret = PSCTRL_DSWIRELESS;
			}
		}

		return ret;
	}

	boolean exitConfigMode () {
		boolean ret = false;

		unsigned long start = millis ();
		do {
			attention ();
			//~ shiftInOut (poll, in, sizeof (poll));
			//~ shiftInOut (exit_config, in, sizeof (exit_config));
			byte *in = autoShift (exit_config, 4);
			noAttention ();

			ret = in != nullptr && !isConfigReply (in);

			if (!ret) {
				delay (COMMAND_RETRY_INTERVAL);
			}
		} while (!ret && millis () - start <= COMMAND_TIMEOUT);
		delay (MODE_SWITCH_DELAY);

		return ret;
	}

	//! @}		// Configuration Mode Functions
	
	//! \name Polling Functions
	//! @{

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
	boolean read () {
		boolean ret = false;

		analogSticksValid = false;
		analogButtonDataValid = false;

		attention ();
		byte *in = autoShift (poll, 3);
		noAttention ();

		if (in != NULL) {
			if (isConfigReply (in)) {
				// We're stuck in config mode, try to get out
				exitConfigMode ();
			} else {
				// We surely have buttons
				previousButtonWord = buttonWord;
				buttonWord = ((PsxButtons) in[4] << 8) | in[3];

				if (isDualShockReply (in) || isFlightstickReply (in)) {
					// We have analog stick data
					analogSticksValid = true;
					rx = in[5];
					ry = in[6];
					lx = in[7];
					ly = in[8];

					if (isDualShock2Reply (in)) {
						// We also have analog button data
						analogButtonDataValid = true;
						for (int i = 0; i < ANALOG_BTN_DATA_SIZE; ++i) {
							analogButtonData[i] = in[i + 9];
						}
					}
				}

				ret = true;
			}
		}

		return ret;
	}

	/** \brief Check if any button has changed state
	 * 
	 * \return true if any button has changed state with regard to the previous
	 *         call to read(), false otherwise
	 */
	boolean buttonsChanged () const {
		return ((previousButtonWord ^ buttonWord) > 0);
	}

	/** \brief Check if a button has changed state
	 * 
	 * \return true if \a button has changed state with regard to the previous
	 *         call to read(), false otherwise
	 */
	boolean buttonChanged (const PsxButtons button) const {
		return (((previousButtonWord ^ buttonWord) & button) > 0);
	}

	/** \brief Check if a button is currently pressed
	 * 
	 * \param[in] button The button to be checked
	 * \return true if \a button was pressed in last call to read(), false
	 *         otherwise
	 */
	boolean buttonPressed (const PsxButton button) const {
		return buttonPressed (~buttonWord, button);
	}

	/** \brief Check if a button is pressed in a Button Word
	 * 
	 * \param[in] buttons The button word to check in
	 * \param[in] button The button to be checked
	 * \return true if \a button is pressed in \a buttons, false otherwise
	 */
	boolean buttonPressed (const PsxButtons buttons, const PsxButton button) const {
		return ((buttons & static_cast<const PsxButtons> (button)) > 0);
	}

	/** \brief Check if a button has just been pressed
	 * 
	 * \param[in] button The button to be checked
	 * \return true if \a button was not pressed in the previous call to read()
	 *         and is now, false otherwise
	 */
	boolean buttonJustPressed (const PsxButton button) const {
		return (buttonChanged (button) & buttonPressed (button));
	}

	/** \brief Check if a button has just been released
	 * 
	 * \param[in] button The button to be checked
	 * \return true if \a button was pressed in the previous call to read() and
	 *         is not now, false otherwise
	 */
	boolean buttonJustReleased (const PsxButton button) const {
		return (buttonChanged (button) & ((~previousButtonWord & button) > 0));
	}

	/** \brief Check if NO button is pressed in a Button Word
	 * 
	 * \param[in] buttons The button word to check in
	 * \return true if all buttons in \a buttons are released, false otherwise
	 */
	boolean noButtonPressed (const PsxButtons buttons) const {
		return buttons == PSB_NONE;
	}

	/** \brief Check if NO button is currently pressed
	 * 
	 * \return true if all buttons were released in the last call to read(),
	 *         false otherwise
	 */
	boolean noButtonPressed (void) const {
		return buttonWord == ~PSB_NONE;
	}
	
	/** \brief Retrieve the <em>Button Word</em>
	 * 
	 * The button word contains the status of all digital buttons and can be
	 * retrieved so that it can be inspected later.
	 * 
	 * \sa buttonPressed
	 * \sa noButtonPressed
	 * 
	 * \return the Button Word
	 */
	PsxButtons getButtonWord () const {
		return ~buttonWord;
	}

	/** \brief Retrieve button pressure depth/strength
	 * 
	 * This function will return how deeply/strongly a button is pressed. It
	 * will only work on DualShock 2 controllers after enabling this feature
	 * with enableAnalogButtons().
	 * 
	 * Note that button pressure depth/strength is only available for the D-Pad
	 * buttons, []/^/O/X, L1/2 and R1/2.
	 *
	 * \param[in] button the button the retrieve the pressure depth/strength of
	 * \return the pressure depth/strength [0-255, Fully released to fully
	 *         pressed]
	 */
	byte getAnalogButton (const PsxAnalogButton button) const {
		byte ret = 0;
		
		if (analogButtonDataValid) {
			ret = analogButtonData[button];
		//~ } else if (buttonPressed (button)) {		// FIXME
			//~ // No analog data, assume fully pressed or fully released
			//~ ret = 0xFF;
		}

		return ret;
	}

	/** \brief Retrieve position of the \a left analog stick
	 * 
	 * This function will return the absolute position of the left analog stick.
	 * 
	 * Note that not all controllers have analog sticks, in which case this
	 * function will return false.
	 * 
	 * \param[in] x A variable where the horizontal position will be stored
	 *              [0-255, L to R]
	 * \param[in] y A variable where the vertical position will be stored
	 *              [0-255, U to D]
	 * \return true if the returned position is valid, false otherwise
	 */
	boolean getLeftAnalog (byte& x, byte& y) const {
		x = lx;
		y = ly;

		return analogSticksValid;
	}

	/** \brief Retrieve position of the \a right analog stick
	 * 
	 * This function will return the absolute position of the right analog
	 * stick.
	 * 
	 * Note that not all controllers have analog sticks, in which case this
	 * function will return false.
	 * 
	 * \param[in] x A variable where the horizontal position will be stored
	 *              [0-255, L to R]
	 * \param[in] y A variable where the vertical position will be stored
	 *              [0-255, U to D]
	 * \return true if the returned position is valid, false otherwise
	 */
	boolean getRightAnalog (byte& x, byte& y) {
		x = rx;
		y = ry;

		return analogSticksValid;
	}
	
	//! @}		// Polling Functions
};

#endif
