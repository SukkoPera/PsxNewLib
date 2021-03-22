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

#ifndef PSXDRIVER_H_
#define PSXDRIVER_H_

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

//~ /** \brief Command timeout (ms)
 //~ * 
 //~ * Commands are sent to the controller repeatedly, until they succeed or time
 //~ * out. This is the length of that timeout.
 //~ * 
 //~ * \sa COMMAND_RETRY_INTERVAL
 //~ */
//~ const unsigned long COMMAND_TIMEOUT = 250;

//~ /** \brief Command Retry Interval (ms)
 //~ * 
 //~ * When sending a command to the controller, if it does not succeed, it is
 //~ * retried after this amount of time.
 //~ */
//~ const unsigned long COMMAND_RETRY_INTERVAL = 10;


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
	 * This can be sized after the longest command reply (which is 21 bytes for
	 * 01 42 when in DualShock 2 mode), but we're better safe than sorry.
	 */
	static const byte BUFFER_SIZE = 32;

	/** \brief Internal communication buffer
	 * 
	 * This is used to hold replies received from the controller.
	 */
	byte inputBuffer[BUFFER_SIZE];

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
	inline static byte getReplyLength (const byte *buf) {
		return (buf[1] & 0x0F) * 2;
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
		// Nothing to do for the moment, but please make sure to call in subclasses
		return true;
	}
};

#endif
