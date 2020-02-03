#include "PsxNewLib.h"
#include <DigitalIO.h>

/** \brief Attention Delay
 *
 * Time between attention being issued to the controller and the first clock
 * edge (us).
 */
const byte ATTN_DELAY = 15;

template <uint8_t PIN_ATT, uint8_t PIN_CMD, uint8_t PIN_DAT, uint8_t PIN_CLK>
class PsxControllerDIOSoftSPI: public PsxController {
private:
	DigitalPin<PIN_UNO_SS> att;
	DigitalPin<PIN_UNO_MOSI> cmd;
	DigitalPin<PIN_UNO_MISO> dat;
	DigitalPin<PIN_UNO_SCK> clk;
	SoftSPI<PIN_UNO_MISO, PIN_UNO_MOSI, PIN_UNO_SCK, /* mode = */ 3> spi;
	
protected:
	virtual void attention () override {
		att.low ();
		delayMicroseconds (ATTN_DELAY);
		spi.begin ();
		dat.config (INPUT, HIGH);     // spi.begin() disables pull-up, re-enable it
	}
	
	virtual void noAttention () override {
		cmd.high ();
		clk.high ();
		att.high ();
		//~ delayMicroseconds (ATTN_DELAY);
	}
	
	virtual byte shiftInOut (const byte out) override {
		return spi.transfer (out);
	}

public:
	virtual boolean begin () override {
		att.config (OUTPUT, HIGH);    // HIGH -> Controller not selected
		cmd.config (OUTPUT, HIGH);
		clk.config (OUTPUT, HIGH);

		return PsxController::begin ();
	}
};
