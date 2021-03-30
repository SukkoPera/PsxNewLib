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
 * \file PsxDriver.h
 * \author SukkoPera <software@sukkology.net>
 * \date 27 Jan 2020
 * \brief Playstation controller driver
 * 
 * Please refer to the GitHub page and wiki for any information:
 * https://github.com/SukkoPera/PsxNewLib
 */

#pragma once

#include "PsxOptions.h"

// Uncomment this to have all byte exchanges logged to serial
//~ #define DUMP_COMMS


/** \brief PSX Driver Interface
 * 
 * This is the base class defining the low-level primitives required for talking
 * to PSX controllers. It is partially abstract, so it is not supposed to be
 * instantiated directly.
 */
class PsxDriver {
protected:
	/** \brief Size of internal communication buffer
	 * 
	 * This can be sized after the longest command reply, which is 32 bytes
	 * (used by the PS1 MultiTap, for instance), plus the usual 3-byte header.
	 */
	static const byte BUFFER_SIZE = 35;

	/** \brief Internal communication buffer
	 * 
	 * This is used to hold replies received from the controller.
	 */
	byte inputBuffer[BUFFER_SIZE];

	/** \brief Time last interaction with the controller took place at.
	 *
	 * We don't want to flood the controller, this helps us behave.
	 */
	unsigned long lastCmdTime;

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

public:
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

	/** \brief Check if the acknowledge pulse was received
	 *
	 * This function must be implemented by derived classes and must return true
	 * after the Acknowledge pulse has been received (i.e.: both the falling and
	 * rising edges have been seen).
	 *
	 * This function MUST NOT block.
	 */
	virtual boolean acknowledged () = 0;

	virtual void selectController () {
		while (millis () - lastCmdTime <= MIN_ATTN_INTERVAL)
			;

		attention ();

		delayMicroseconds (ATTN_DELAY);
	}
	
	virtual void deselectController () {
		noAttention ();

		lastCmdTime = millis ();
	}
	
	
	/** \brief Transfer several bytes to/from the controller
	 * 
	 * This function transfers an array of <i>command</i> bytes to the
	 * controller and reads back an equally sized array of <i>data</i> bytes.
	 * 
	 * \param[in] out The command bytes to send the controller
	 * \param[in] outLen Length of \a out, might be less than \a in, in which
	 *                   case padding bytes are generated automatically
	 * \param[out] in The data bytes returned by the controller, must be sized
	 *                 to hold at least \a len bytes
	 * \param[in] len The amount of bytes to be exchanged
	 * \param[in] needLastAck true if the last byte send must be acknowledged
	 *                        (i.e. if other bytes will follow)
	 * \return true if the transmission took place correctly (i.e.: all bytes
	 *         were acknowledged)
	 */
	boolean shiftInOut (const byte *out, const byte outLen,
	                    byte *in,  const byte len,
	                    const boolean needLastAck) {
							
		boolean ret = true;
#ifdef DUMP_COMMS
		byte inbuf[len];
#endif

		for (byte i = 0; i < len; ++i) {
			byte tmp = shiftInOut (out != NULL && i < outLen ? out[i] : PADDING_BYTE);
#ifdef DUMP_COMMS
			inbuf[i] = tmp;
#endif
			if (in != NULL) {
				in[i] = tmp;
			}

			if (i < len - 1 || needLastAck) {
				unsigned long start = micros ();
				while (!acknowledged () && micros () - start < INTER_CMD_BYTE_TIMEOUT)
					;
				if (!acknowledged ()) {
					ret = false;
				}
			}
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
		if (!ret) {
			Serial.println (F("!ACK"));
		} else {
			Serial.println ();
		}
#endif

		return ret;
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
		boolean txOk = false;
		
		if (len >= 3 && len <= BUFFER_SIZE) {
			// All commands have at least 3 bytes, so shift out those first
			txOk = shiftInOut (out, 3, inputBuffer, 3, len > 3);
			if (txOk && isValidReply (inputBuffer)) {
				/* Reply is good, calculate length. This won't include the 3
				 * bytes we have already send, so it's basically the number of
				 * bytes we still have to exchange.
				 */
				const byte replyLen = getReplyLength (inputBuffer);
				if (replyLen > 0) {
					// Shift out rest of command
					if (replyLen <= BUFFER_SIZE - 3) {
						// Part of reply is still missing and we have space for it
						txOk = shiftInOut (out + 3, len - 3, inputBuffer + 3, replyLen, false);
					} else {
						// Reply incomplete but not enough space available
						txOk = false;
					}
				} else {
					// The whole reply was gathered, txOk is already true
				}
			} else {
				txOk = false;
			}
		}

		return txOk ? inputBuffer : nullptr;
	}

	/** \brief Get reply length
	 * 
	 * Calculates the length of a command reply, in bytes
	 * 
	 * \param[in] buf The buffer containing the reply, must be at least 2 bytes
	 *                long
	 * \return The calculated length
	 */
	inline static byte getReplyLength (const byte *buf) {
		const byte n = buf[1] & 0x0F;
		return (n == 0 ? 16 : n) * 2;
	}

	inline static boolean isValidReply (const byte *status) {
		//~ return status[0] != 0xFF || status[1] != 0xFF || status[2] != 0xFF;
		return status[1] != 0xFF && (status[2] == 0x5A || status[2] == 0x00);
		//~ return /* status[0] == 0xFF && */ status[1] != 0xFF && status[2] == 0x5A;
	}

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
		// Not much to do for the moment, but please make sure to call in subclasses
		lastCmdTime = 0;
		
		return true;
	}
};
