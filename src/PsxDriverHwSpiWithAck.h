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
 * \file PsxDriverHwSpi.h
 * \author SukkoPera <software@sukkology.net>
 * \date 22 Mar 2021
 * \brief Playstation Controller Hardware SPI Driver
 * 
 * Please refer to the GitHub page and wiki for any information:
 * https://github.com/SukkoPera/PsxNewLib
 */

#include "PsxDriver.h"
#include <SPI.h>
#include <DigitalIO.h>

// Set up the speed, data order and data mode
static SPISettings spiSettings (250000, LSBFIRST, SPI_MODE3);

// This needs to be here because it is accessed by the ISR
static volatile byte ackReceived;

/* The ACK signal is very hard to handle correctly: it is normally high but goes
 * low for ~2us at the end of every byte (except the last one, so it looks more
 * like a "I'm ready for the next one" rather than a "I got the last one
 * correctly", but anyway...).
 *
 * Polling is out of the question, and by using a PCINT (which we prefer over
 * external interrupts for flexibility on the pin choice) we end up missing the
 * rising edge too often. The only reliable thing seems to be a mix of the two,
 * i.e.: wait for an interrupt and then for the line to get back high. This
 * seems to work fine with both controllers and the MultiTap.
 */
template <uint8_t PIN_ATT, uint8_t PIN_ACK>
class PsxDriverHwSpiWithAck: public PsxDriver {
private:
	DigitalPin<PIN_ATT> att;
	DigitalPin<PIN_ACK> ack;
	DigitalPin<MOSI> cmd;
	DigitalPin<MISO> dat;
	DigitalPin<SCK> clk;

	// Low-level stuff for interrupt on ACK signal
	volatile byte *pcicrReg;
	byte pcicrBitMask;
	volatile byte *pcintMaskReg;
	byte pcintMaskValue;

protected:
	virtual byte shiftInOut (const byte out) override {
		PCIFR |= pcicrBitMask;	// Clear any pending interrupt
		ackReceived = 0;		// Reset edge count
		return SPI.transfer (out);
	}

public:
	virtual void attention () override {
		att.low ();

		SPI.beginTransaction (spiSettings);

		// Enable ACK interrupt
		*pcicrReg |= pcicrBitMask;
		*pcintMaskReg |= pcintMaskValue;
	}
	
	virtual void noAttention () override {
		SPI.endTransaction ();

		// Don't trigger ACK interrupt anymore
		*pcicrReg &= ~pcicrBitMask;
		*pcintMaskReg &= ~pcintMaskValue;

		// Make sure CMD and CLK sit high
		cmd.high ();    // This actually does nothing as pin stays under SPI control, I guess
		clk.high ();    // Ditto
		att.high ();
	}

	virtual boolean acknowledged () override {
		return ackReceived > 0 && ack;
	}
	
	virtual boolean begin () override {
		att.config (OUTPUT, HIGH);    // HIGH -> Controller not selected

		/* We need to force these at startup, that's why we need to know which
		 * pins are used for HW SPI. It's a sort of "start condition" the
		 * controller needs.
		 */
		cmd.config (OUTPUT, HIGH);
		clk.config (OUTPUT, HIGH);
		dat.config (INPUT, HIGH);     // Enable pull-up

		SPI.begin ();
				
		// Pin-Change Interrupt control registers for ACK signal
		pcicrReg = digitalPinToPCICR (PIN_ACK);
		pcicrBitMask = 1 << digitalPinToPCICRbit (PIN_ACK);
		pcintMaskReg = digitalPinToPCMSK (PIN_ACK);
		pcintMaskValue = 1 << digitalPinToPCMSKbit (PIN_ACK);

		return PsxDriver::begin ();
	}
};

// This ISR approach was lifted from the Arduino SoftwareSerial library
#if defined(PCINT0_vect)
ISR (PCINT0_vect) {
	/* Erm, I guess we should check whether it's our pin that changed level?
	 * Users might be using other PCINTs for their own purposes... But how can
	 * we do it? We are too slow...
	 *
	 * And how could they have their own ISR?
	 */
	++ackReceived;
}
#endif

#if defined(PCINT1_vect)
ISR (PCINT1_vect, ISR_ALIASOF (PCINT0_vect));
#endif

#if defined(PCINT2_vect)
ISR (PCINT2_vect, ISR_ALIASOF (PCINT0_vect));
#endif

#if defined(PCINT3_vect)
ISR (PCINT3_vect, ISR_ALIASOF (PCINT0_vect));
#endif
