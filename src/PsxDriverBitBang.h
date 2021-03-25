#include "PsxDriver.h"
#include <DigitalIO.h>

/** \brief Attention Delay
 *
 * Time between attention being issued to the controller and the first clock
 * edge (us).
 */
const byte ATTN_DELAY = 15;

/** \brief Clock Period
 *
 * Inverse of clock frequency, i.e.: time for a *full* clock cycle, from falling
 * edge to the next falling edge.
 */
const byte CLK_PERIOD = 6;



template <uint8_t PIN_ATT, uint8_t PIN_CMD, uint8_t PIN_DAT, uint8_t PIN_CLK>
class PsxDriverBitBang: public PsxDriver {
private:
	DigitalPin<PIN_ATT> att;
	DigitalPin<PIN_CLK> clk;
	DigitalPin<PIN_CMD> cmd;
	DigitalPin<PIN_DAT> dat;

protected:
	virtual byte shiftInOut (const byte out) override {
		byte in = 0;

		// 1. The clock is held high until a byte is to be sent.

		for (byte i = 0; i < 8; ++i) {
			// 2. When the clock edge drops low, the values on the line start to
			// change
			clk.low ();
			if (bitRead (out, i)) {
				cmd.high ();
			} else {
				cmd.low ();
			}

			delayMicroseconds (CLK_PERIOD / 2);

			// 3. When the clock goes from low to high, value are actually read
			clk.high ();
			if (dat) {
				bitSet (in, i);
			}

			delayMicroseconds (CLK_PERIOD / 2);
		}

		return in;
	}

public:
	virtual void attention () override {
		att.low ();
		delayMicroseconds (ATTN_DELAY);
	}
	
	virtual void noAttention () override {
		cmd.high ();
		clk.high ();
		att.high ();
		delayMicroseconds (ATTN_DELAY);
	}
	
	virtual boolean begin () override {
		att.config (OUTPUT, HIGH);    // HIGH -> Controller not selected
		cmd.config (OUTPUT, HIGH);
		clk.config (OUTPUT, HIGH);
		dat.config (INPUT, HIGH);     // Enable pull-up

		return PsxDriver::begin ();
	}
};
