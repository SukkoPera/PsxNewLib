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
 * \file PsxOptions.h
 * \author SukkoPera <software@sukkology.net>
 * \date 22 Mar 2021
 * \brief Playstation Controller Misc Options
 * 
 * Please refer to the GitHub page and wiki for any information:
 * https://github.com/SukkoPera/PsxNewLib
 */

#pragma once

#include <Arduino.h>

/** \brief Attention Delay
 *
 * Time between attention being issued to the controller and the first clock
 * edge (us).
 */
const byte ATTN_DELAY = 15;

/** \brief Minimum Attention Interval
 *
 * PSX controllers were only designed to be interacted with once per frame. We
 * don't want to flood them with communication requests, so this is the minimum
 * interval that must elapse between the end of a command and the beginning of
 * the following one (us).
 * 
 * Note that maybe configuration commands can be send more rapidly, but we
 * always wait at the moment.
 */
const byte MIN_ATTN_INTERVAL = 1000 / 60;

/** \brief Command Inter-Byte Timeout (us)
 * 
 * Commands are several bytes long. After every byte, the controller is supposed
 * to send an \a Acknowledge signal. This is the maximum time to wait for that
 * signal.
 * 
 * NOTE: Not all drivers watch the \a Acknowledge line, some just wait a fixed
 * amount of time.
 *
 * \sa INTER_CMD_BYTE_DELAY
 */
const byte INTER_CMD_BYTE_TIMEOUT = 100;

/** \brief Command Inter-Byte Delay (us)
 * 
 * For drivers that do not watch the \a Acknowledge line, this is the time to
 * wait between two consecutive bytes.
 *
 * \sa INTER_CMD_BYTE_TIMEOUT
 */
const byte INTER_CMD_BYTE_DELAY = 50;

/** \brief Padding byte value
 *
 * The value sent to the controller when we must generate padding bytes. This is
 * usually 0x5A or 0x00.
 */
const byte PADDING_BYTE = 0x5A;

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
