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
 * \file PsxTypes.h
 * \author SukkoPera <software@sukkology.net>
 * \date 22 Mar 2021
 * \brief Playstation Controller Interface Types
 * 
 * Please refer to the GitHub page and wiki for any information:
 * https://github.com/SukkoPera/PsxNewLib
 */

#ifndef PSX_PUBTYPES_H_
#define PSX_PUBTYPES_H_

#include <Arduino.h>

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

/** \brief Number of digital buttons
 *
 * Includes \a everything, i.e.: 4 directions, Square, Cross, Circle, Triangle,
 * L1/2/3, R1/2/3, Select and Start.
 *
 * This is the number of entries in #PsxButton.
 */
const byte PSX_BUTTONS_NO = 16;

/** \brief Type that is used to report button presses
 */
typedef uint16_t PsxButtons;

/** \brief Controller Type
 *
 * This is somehow derived from the reply to the #type_read command. It is NOT
 * much trustworthy, so it might be removed in the future.
 *
 * \sa getControllerType
 */
enum PsxControllerType {
	PSCTRL_UNKNOWN = 0,			//!< No idea
	PSCTRL_DUALSHOCK,			//!< DualShock or compatible
	PSCTRL_DSWIRELESS,			//!< Sony DualShock Wireless
	PSCTRL_GUITHERO,			//!< Guitar Hero controller
};

/** \brief Number of different controller types recognized
 *
 * This is the number of entries in #PsxControllerType.
 */
const byte PSCTRL_MAX = static_cast<byte> (PSCTRL_GUITHERO) + 1;


/** \brief Controller Protocol
 *
 * Identifies the protocol the controller uses to report axes positions and
 * button presses. It's quite more reliable than #PsxControllerType, so use this
 * if you must.
 *
 * \sa getProtocol
 */
enum PsxControllerProtocol {
	PSPROTO_UNKNOWN = 0,		//!< No idea
	PSPROTO_DIGITAL,			//!< Original controller (SCPH-1010) protocol (8 digital buttons + START + SELECT)
	PSPROTO_DUALSHOCK,			//!< DualShock (has analog axes)
	PSPROTO_DUALSHOCK2,			//!< DualShock 2 (has analog axes and buttons)
	PSPROTO_FLIGHTSTICK,		//!< Green-mode (like DualShock but missing SELECT, L3 and R3)
	PSPROTO_NEGCON,				//!< Namco neGcon (has 1 analog X axis and analog Square, Circle and L1 buttons)
	PSPROTO_JOGCON				//!< Namco Jogcon (Wheel is mapped to analog X axis, half a rotation in each direction)
};

/** \brief Number of different protocols supported
 *
 * This is the number of entries in #PsxControllerProtocol.
 */
const byte PSPROTO_MAX = static_cast<byte> (PSPROTO_JOGCON) + 1;

/** \brief Analog sticks minimum value
 * 
 * Minimum value reported by analog sticks. This usually means that the stick is
 * fully either at the top or left position. Note that some sticks might not get
 * fully down to this value.
 *
 * \sa ANALOG_MAX_VALUE
 * \sa ANALOG_IDLE_VALUE
 */
const byte ANALOG_MIN_VALUE = 0U;

/** \brief Analog sticks maximum value
 * 
 * Maximum value reported by analog sticks. This usually means that the stick is
 * fully either at the bottom or right position. Note that some sticks might not
 * get fully up to this value.
 *
 * \sa ANALOGI_MAX_VALUE
 * \sa ANALOG_IDLE_VALUE
 */
const byte ANALOG_MAX_VALUE = 255U;

/** \brief Analog sticks idle value
 * 
 * Value reported when an analog stick is in the (ideal) center position. Note
 * that old and worn-out sticks might not self-center perfectly when released,
 * so you should never rely on this precise value to be reported.
 *
 * Also note that the up/down and left/right ranges are off by one, since values
 * 0-127 represent up/left and 129-255 mean down/right. The former interval
 * contains 128 different values, while the latter only 127. Sometimes you will
 * need to take this in consideration.
 */
const byte ANALOG_IDLE_VALUE = 128U;


/** \brief Size of buffer holding analog button data
 *
 * This is the size of the array returned by getAnalogButtonData().
 */
const byte PSX_ANALOG_BTN_DATA_SIZE = 12;

struct PsxSingleController {
		/** \brief Previous (Digital) Button Status
	 * 
	 * The individual bits can be identified through #PsxButton.
	 */
	PsxButtons previousButtonWord;

	/** \brief (Digital) Button Status
	 * 
	 * The individual bits can be identified through #PsxButton.
	 */
	PsxButtons buttonWord;

	/** \brief Controller Protocol
	 *
	 * The protocol controller data was interpreted with at the last call to
	 * read()
	 *
	 * \sa getProtocol
	 */
	PsxControllerProtocol protocol;

	//! \name Analog Stick Data
	//! @{
	byte lx;		//!< Horizontal axis of left stick [0-255, L to R]
	byte ly;		//!< Vertical axis of left stick [0-255, U to D]
	byte rx;		//!< Horizontal axis of right stick [0-255, L to R]
	byte ry;		//!< Vertical axis of right stick [0-255, U to D]
	
	boolean analogSticksValid;	//!< True if the above were valid at the last call to read()
	//! @}
	
	/** \brief Analog Button Data
	 * 
	 * \todo What's the meaning of every individual byte?
	 */
	byte analogButtonData[PSX_ANALOG_BTN_DATA_SIZE];

	/** \brief Analog Button Data Validity
	 * 
	 * True if the #analogButtonData were valid in last call to read()
	 */
	boolean analogButtonDataValid;

	void clear () {
		previousButtonWord = ~0;
		buttonWord = ~0;
		
		// Start with all analog axes at midway position
		lx = ANALOG_IDLE_VALUE;		
		ly = ANALOG_IDLE_VALUE;
		rx = ANALOG_IDLE_VALUE;
		ry = ANALOG_IDLE_VALUE;

		analogSticksValid = false;
		memset (analogButtonData, 0, sizeof (analogButtonData));

		protocol = PSPROTO_UNKNOWN;
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
	 * \sa getPreviousButtonWord
	 * 
	 * \return the Button Word
	 */
	PsxButtons getButtonWord () const {
		return ~buttonWord;
	}
	
	/** \brief Retrieve the <em>Previous Button Word</em>
	 * 
	 * The previous button word contains the status of all digital buttons at
	 * the read before the current one and can be retrieved so that it can be
	 * inspected later.
	 * 
	 * \sa getButtonWord
	 * 
	 * \return the Previous Button Word
	 */
	PsxButtons getPreviousButtonWord () const {
		return ~previousButtonWord;
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

	/** \brief Retrieve all analog button data
	 */
	const byte* getAnalogButtonData () const {
		return analogButtonDataValid ? analogButtonData : NULL;
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
};

#endif
