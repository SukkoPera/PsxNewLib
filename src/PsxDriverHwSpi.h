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


template <uint8_t PIN_ATT>
class PsxDriverHwSpi: public PsxDriver {
private:
	DigitalPin<PIN_ATT> att;
	DigitalPin<MOSI> cmd;
	DigitalPin<MISO> dat;
	DigitalPin<SCK> clk;

	unsigned long byteFinishTime;

	// Set up the speed, data order and data mode
	static const SPISettings spiSettings;

protected:
	virtual byte shiftInOut (const byte out) override {
		byte b = SPI.transfer (out);
		byteFinishTime = micros ();

		return b;
	}

public:
	virtual void attention () override {
		att.low ();

		SPI.beginTransaction (spiSettings);
	}
	
	virtual void noAttention () override {
		SPI.endTransaction ();

		// Make sure CMD and CLK sit high
		cmd.high ();    // This actually does nothing as pin stays under SPI control, I guess
		clk.high ();    // Ditto
		att.high ();
	}

	virtual boolean acknowledged () override {
		// We just wait a bit, hoping the ACK goes by...
		return micros () - byteFinishTime > INTER_CMD_BYTE_DELAY;
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

		return PsxDriver::begin ();
	}
};

// Init static data members
template <uint8_t PIN_ATT>
const SPISettings PsxDriverHwSpi<PIN_ATT>::spiSettings (250000, LSBFIRST, SPI_MODE3);
