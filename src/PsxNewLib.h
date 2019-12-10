//~ #define DUMP_COMMS

// us
const byte INTER_CMD_BYTE_DELAY = 15;

// ms
const unsigned long COMMAND_TIMEOUT = 250;

// ms
const unsigned long COMMAND_RETRY_INTERVAL = 10;

// ms
const unsigned long MODE_SWITCH_DELAY = 500;


/** \brief Type that is used to report button presses
 */
enum PsxButton {
	PSB_NONE       = 0x0000,
	PSB_SELECT     = 0x0001,
	PSB_L3         = 0x0002,
	PSB_R3         = 0x0004,
	PSB_START      = 0x0008,
	PSB_PAD_UP     = 0x0010,
	PSB_PAD_RIGHT  = 0x0020,
	PSB_PAD_DOWN   = 0x0040,
	PSB_PAD_LEFT   = 0x0080,
	PSB_L2         = 0x0100,
	PSB_R2         = 0x0200,
	PSB_L1         = 0x0400,
	PSB_R1         = 0x0800,
	PSB_GREEN      = 0x1000,
	PSB_RED        = 0x2000,
	PSB_BLUE       = 0x4000,
	PSB_PINK       = 0x8000,
	PSB_TRIANGLE   = 0x1000,
	PSB_CIRCLE     = 0x2000,
	PSB_CROSS      = 0x4000,
	PSB_SQUARE     = 0x8000
};

typedef uint16_t PsxButtons;

static byte enter_config[] = {0x01, 0x43, 0x00, 0x01, 0x00};
static byte set_mode[] = {0x01, 0x44, 0x00, /* enabled */ 0x01, /* locked */ 0x03, 0x00, 0x00, 0x00, 0x00};
static byte set_pressures[] = {0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
//~ static byte exit_config[] = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
static byte exit_config[] = {0x01, 0x43, 0x00, 0x00, 0x00};
//~ static byte enable_rumble[] = {0x01, 0x4D, 0x00, 0x00, 0x01};
static byte type_read[] = {0x01, 0x45, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
static byte poll[] = {0x01, 0x42, 0x00, 0xFF, 0xFF};

enum PsxControllerType {
	PSCTRL_UNKNOWN = 0,
	PSCTRL_DUALSHOCK,
	PSCTRL_DSWIRELESS,
	PSCTRL_GUITHERO,
	
	PSCTRL_MAX
};

class PsxController {
protected:
	boolean analogMode;
	
	PsxButtons buttonWord;

	byte lx;
	byte ly;
	byte rx;
	byte ry;

	virtual void attention () = 0;
	
	virtual void noAttention () = 0;
	
	virtual byte shiftInOut (const byte out) = 0;

	void shiftInOut (const byte *out, byte *in, const byte len) {
#ifdef DUMP_COMMS
		byte inbuf[len];
#endif

		for (byte i = 0; i < len; ++i) {
			byte tmp = shiftInOut (out[i]);
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
			if (out[i] < 0x10)
				Serial.print (0);
			Serial.print (out[i], HEX);
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

	inline boolean isValidReply (const byte *status) {
		return status[0] != 0xFF || status[1] != 0xFF || status[2] != 0xFF;
	}
	
	inline boolean isAnalogReply (const byte *status) {
		return (status[1] & 0xF0) == 0x70;
	}
	
	inline boolean isDigitalReply (const byte *status) {
		return (status[1] & 0xF0) == 0x40;
	}
	
	inline boolean isConfigReply (const byte *status) {
		return status[1] == 0xF3 /* && status[2] == 0x5A */;
	}
	
public:
	virtual boolean begin () {
		lx = 0;
		ly = 0;
		rx = 0;
		ry = 0;

		analogMode = false;
		
		// Some disposable readings to let the controller know we are here
		for (byte i = 0; i < 5; ++i) {
			read ();
			delay (1);
		}

		return read ();
	}
	
	PsxButtons getButtonWord () const {
		return ~buttonWord;
	}

	boolean getLeftAnalog (byte& x, byte& y) {
		x = lx;
		y = ly;

		return analogMode;
	}

	boolean getRightAnalog (byte& x, byte& y) {
		x = rx;
		y = ry;

		return analogMode;
	}

	boolean buttonPressed (PsxButtons buttonWordx, PsxButton button) {
		return ((buttonWordx & static_cast<PsxButtons> (button)) > 0);
	}

	boolean buttonPressed (PsxButton button) {
		return buttonPressed (~buttonWord, button);
	}

	boolean noButtonPressed (PsxButtons buttons) {
		return buttons == PSB_NONE;
	}
	
	boolean noButtonPressed (void) {
		return buttonWord == ~PSB_NONE;
	}
	
	boolean enterConfigMode () {
		boolean ret = false;
		byte in[sizeof (enter_config)];

		unsigned long start = millis ();
		do {
			attention ();
			shiftInOut (enter_config, in, sizeof (enter_config));
			noAttention ();

			ret = isValidReply (in) && isConfigReply (in);

			if (!ret) {
				delay (COMMAND_RETRY_INTERVAL);
			}
		} while (!ret && millis () - start <= COMMAND_TIMEOUT);
		delay (MODE_SWITCH_DELAY);

		return ret;
	}
	
	boolean setAnalogMode (bool enabled = true, bool locked = false) {
		boolean ret = false;
		byte out[sizeof (set_mode)];
		byte in[sizeof (set_mode)];
		
		memcpy (out, set_mode, sizeof (set_mode));
		out[3] = enabled ? 0x01 : 0x00;
		out[4] = locked ? 0x03 : 0x00;
		
		attention ();
		shiftInOut (out, in, sizeof (set_mode));
		noAttention ();
		
		// Give controller some time to switch to set the requested mode
		delay (MODE_SWITCH_DELAY);

		unsigned long start = millis ();
		byte cnt = 0;
		do {
			attention ();
			shiftInOut (out, in, sizeof (set_mode));
			noAttention ();

			/* We can't know if we have successfully enabled analog mode until
			 * we get out of config mode, so let's just be happy if we get a few
			 * consecutive valid replies
			 */
			if (isValidReply (in)) {
				++cnt;
			}
			ret = cnt >= 3;

			if (!ret) {
				delay (COMMAND_RETRY_INTERVAL);
			}
		} while (!ret && millis () - start <= COMMAND_TIMEOUT);
		delay (MODE_SWITCH_DELAY);
		
		return ret;
	}
	
	PsxControllerType getControllerType () {
		byte in[sizeof (type_read)];
		
		attention ();
		shiftInOut (type_read, in, sizeof (type_read));
		noAttention ();
		
		const byte& controllerType = in[3];
		if (controllerType == 0x03) {
			return PSCTRL_DUALSHOCK;
		//~ } else if (controllerType == 0x01 && in[1] == 0x42) {
			//~ return 4;		// ???
		}  else if (controllerType == 0x01 && in[1] != 0x42) {
			return PSCTRL_GUITHERO;
		} else if (controllerType == 0x0C) {
			return PSCTRL_DSWIRELESS;
		}
		
		return PSCTRL_UNKNOWN;
	}
	
	void enablePressures () {
		byte in[sizeof (set_pressures)];
		
		attention ();
		shiftInOut (set_pressures, in, sizeof (set_pressures));
		noAttention ();
		
		//~ delay (1000);
	}
	
	boolean exitConfigMode () {
		boolean ret = false;
		byte in[sizeof (exit_config)];

		unsigned long start = millis ();
		do {
			attention ();
			//~ shiftInOut (poll, in, sizeof (poll));
			shiftInOut (exit_config, in, sizeof (enter_config));
			noAttention ();
			
			ret = isValidReply (in) && !isConfigReply (in);

			if (!ret) {
				delay (COMMAND_RETRY_INTERVAL);
			}
		} while (!ret && millis () - start <= COMMAND_TIMEOUT);
		delay (MODE_SWITCH_DELAY);
		
		return ret;
	}

	boolean read () {
		boolean ret = false;
		byte in[21];

		attention ();
		shiftInOut (poll, in, sizeof (poll));
		
		if (isAnalogReply (in)) {
			// If controller is in full data mode, get the rest of data

			// Just send zeros
			byte tmp[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
			shiftInOut (tmp, in + sizeof (poll), sizeof (tmp));
			
			ret = true;
		} else if (isDigitalReply (in)) {
			ret = true;
		}
		
		noAttention ();

		if (ret) {
			buttonWord = ((PsxButtons) in[4] << 8) | in[3];

			if (isAnalogReply (in)) {
				analogMode = true;
				rx = in[5];
				ry = in[6];
				lx = in[7];
				ly = in[8];
			}
		} else if (isConfigReply (in)) {
			// We're stuck in config mode, try to get out
			exitConfigMode ();
		} else {
			analogMode = false;
		}
	 
		return ret;
	}
};
