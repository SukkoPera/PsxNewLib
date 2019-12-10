#include "PsxNewLib.h"
#include <SPI.h>
#include <DigitalIO.h>

/** \brief Attention Delay
 *
 * Time between attention being issued to the controller and the first clock
 * edge.
 */
const byte ATTN_DELAY = 8;

const byte PIN_UNO_SS = 10;
const byte PIN_UNO_MOSI = 11;
const byte PIN_UNO_MISO = 12;
const byte PIN_UNO_SCK = 13;

// Set up the speed, data order and data mode
static SPISettings spiSettings (25000, LSBFIRST, SPI_MODE3);

class PsxControllerHwSpi: public PsxController {
private:
	DigitalPin<PIN_UNO_SS> att;
	DigitalPin<PIN_UNO_MOSI> cmd;
	DigitalPin<PIN_UNO_MISO> dat;
	DigitalPin<PIN_UNO_SCK> clk;

protected:
	virtual void attention () override {
		att.low ();

		SPI.beginTransaction (spiSettings);

		delayMicroseconds (ATTN_DELAY);
	}
	
	virtual void noAttention () override {
		//~ delayMicroseconds (5);
		
		SPI.endTransaction ();

		cmd.high ();
		clk.high ();
		att.high ();
		delayMicroseconds (ATTN_DELAY);
	}
	
	virtual byte shiftInOut (const byte out) override {
		return SPI.transfer (out);
	}

public:
	virtual boolean begin () override {
		att.config (OUTPUT, HIGH);    // HIGH -> Controller not selected
		cmd.config (OUTPUT, HIGH);
		clk.config (OUTPUT, HIGH);
		dat.config (INPUT, HIGH);     // Enable pull-up

		SPI.begin ();

		return PsxController::begin ();
	}
};
