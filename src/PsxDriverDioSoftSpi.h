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
 * \file PsxDriverDioSoftSpi.h
 * \author SukkoPera <software@sukkology.net>
 * \date 22 Mar 2021
 * \brief Playstation Controller Software SPI Driver
 * 
 * Please refer to the GitHub page and wiki for any information:
 * https://github.com/SukkoPera/PsxNewLib
 */

#include "PsxDriver.h"
#include <DigitalIO.h>

/** \brief Attention Delay
 *
 * Time between attention being issued to the controller and the first clock
 * edge (us).
 */
const byte ATTN_DELAY = 15;

template <uint8_t PIN_ATT, uint8_t PIN_CMD, uint8_t PIN_DAT, uint8_t PIN_CLK>
class PsxDriverDioSoftSpi: public PsxDriver {
private:
	DigitalPin<PIN_ATT> att;
	DigitalPin<PIN_CMD> cmd;
	DigitalPin<PIN_DAT> dat;
	DigitalPin<PIN_CLK> clk;
	SoftSPI<PIN_DAT, PIN_CMD, PIN_CLK, /* mode = */ 3> spi;

protected:
	// https://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
	static inline byte reverse (byte b) {
		b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
		b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
		b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
		return b;
	}

	virtual byte shiftInOut (const byte out) override {
		// SoftSPI only works MSBFIRST, so we need to play around a bit
		return reverse (spi.transfer (reverse (out)));
	}

public:
	virtual void attention () override {
		att.low ();
		delayMicroseconds (ATTN_DELAY);
		//~ cmd.high ();
		//~ clk.high ();
		//~ dat.high ();     // spi.begin() disables pull-up, re-enable it
	}

	virtual void noAttention () override {
		cmd.high ();
		clk.high ();
		att.high ();
		//~ delayMicroseconds (ATTN_DELAY);
	}
	
	virtual boolean begin () override {
		att.config (OUTPUT, HIGH);    // HIGH -> Controller not selected
		cmd.config (OUTPUT, HIGH);
		clk.config (OUTPUT, HIGH);

		spi.begin ();

		return PsxDriver::begin ();
	}
};
