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
 * \brief Playstation Controller Misc Options
 * 
 * Please refer to the GitHub page and wiki for any information:
 * https://github.com/SukkoPera/PsxNewLib
 */

#include <Arduino.h>

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

/** \brief neGcon I/II-button press threshold
 *
 * The neGcon does not report digital button press data for its analog buttons,
 * so we have to make it up. The Square, Cross digital buttons will be
 * reported as pressed when the analog value of the II and I buttons
 * (respectively), goes over this threshold.
 *
 * \sa NEGCON_L_BUTTON_THRESHOLD
 */
const byte NEGCON_I_II_BUTTON_THRESHOLD = 128U;

/** \brief neGcon L-button press threshold
 *
 * The neGcon does not report digital button press data for its analog buttons,
 * so we have to make it up. The L1 digital button will be reported as pressed
 * when the analog value of the L buttons goes over this threshold.
 *
 * This value has been tuned so that the L button gets digitally triggered at
 * about the same point as the non-analog R button. This is done "empirically"
 * and might need tuning on a different controller than the one I actually have.
 * 
 * \sa NEGCON_I_II_BUTTON_THRESHOLD
 */
const byte NEGCON_L_BUTTON_THRESHOLD = 240U;
