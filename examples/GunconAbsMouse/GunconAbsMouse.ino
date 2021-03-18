/*******************************************************************************
 * This sketch shows how the library can be used to turn a PSX Guncon controller
 * into an USB mouse, using an Arduino Leonardo.
 * It uses an edited version of AbsMouse Library.
 *
 * For details on AbsMouse, see
 * https://github.com/jonathanedgecombe/absmouse
 * 
 * The guncon needs to "scan" the entire screen before it can properly send
 * the coorinates. Just point it at the screen and move slosly from side to side
 * and top to botom. The values will be stored as min and max, and will be used
 * to calculate the absolute mouse position.
 */

/*
Buttons are mapped as follows:
A (Left side) -> Start PSB_START
B (Right side) -> Cross PSB_CROSS
Trigger -> Circle PSB_CIRCLE
*/

#include <PsxControllerBitBang.h>
#include "AbsMouse.h"

/* We must use the bit-banging interface, as SPI pins are only available on the
 * ICSP header on the Leonardo.
*/
const byte PIN_PS2_ATT = 10;
const byte PIN_PS2_CMD = 11;
const byte PIN_PS2_DAT = 12;
const byte PIN_PS2_CLK = 13;

const byte PIN_BUTTONPRESS = A0;

const unsigned long POLLING_INTERVAL = 1000U / 50U;

// Send debug messages to serial port
#define ENABLE_SERIAL_DEBUG

PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;

#ifdef ENABLE_SERIAL_DEBUG
//	#define dstart(spd) do {Serial.begin (spd); while (!Serial) {digitalWrite (LED_BUILTIN, (millis () / 500) % 2);}} while (0);
  #define dstart(spd) do {Serial.begin (spd); while (!Serial) {digitalWrite (PIN_BUTTONPRESS, (millis () / 500) % 2);}} while (0);
	#define debug(...) Serial.print (__VA_ARGS__)
	#define debugln(...) Serial.println (__VA_ARGS__)
#else
	#define dstart(...)
  #define debug(...)
  #define debugln(...)
#endif

boolean haveController = false;

// Minimum and maximum detected values. Varies from tv to tv.
word minX = 1000;
word maxX = 0;
word minY = 1000;
word maxY = 0;

// Last successful read coordinates
word lastX = -1;
word lastY = -1;

boolean btnCircleWasPressed = false;
boolean btnCrossWasPressed = false;
boolean btnStartWasPressed = false;

boolean enableMouseMove = true;

// Translate guncon values to the mouse absolute values (zero to 32767).
word convertRange(word gcMin, word gcMax, word value)
{
  word scale = (word)(32767) / (gcMax - gcMin);
  return (word)((value - gcMin) * scale);
}


void setup () {
  // Init AbsMouse library
  AbsMouse.init();

	dstart (115200);

	debugln (F("Ready!"));
}

void loop () {
	static unsigned long last = 0;
	
	if (millis () - last >= POLLING_INTERVAL) {
		last = millis ();
		
		if (!haveController) {
			if (psx.begin ()) {
				debugln (F("Controller found!"));
       
				haveController = true;
			}
		} else {
			if (!psx.read ()) {
				//debugln (F("Controller lost :("));
        debug (F("Controller lost."));
        debug (F(" last values: x = "));
        debug (lastX);
        debug (F(", y = "));
        debugln (lastY);
        
				haveController = false;
			} else {
        // Read was successful, so let's make up data for Mouse
        
        word x, y;
        GunconStatus gcStatus;

        //Handle trigger press/release. Maps to left mouse button.
        if(psx.buttonPressed (PSB_CIRCLE)) {
            if(!btnCircleWasPressed) {
              debugln (F("Trigger press"));
              AbsMouse.press(MOUSE_LEFT);
            }
            btnCircleWasPressed = true;
        } else {
          if (btnCircleWasPressed) {
            debugln (F("Trigger release"));
            AbsMouse.release(MOUSE_LEFT);
          }
          btnCircleWasPressed = false;
        }
        
        //Handle btn A press/release. Enables/disables gun to mouse movement
        if(psx.buttonPressed (PSB_START)) {
            if(!btnStartWasPressed) {
              debugln (F("Btn A press"));
              enableMouseMove = !enableMouseMove;
            }
            btnStartWasPressed = true;
        } else {
          if (btnStartWasPressed)
            debugln (F("Btn A release"));
          btnStartWasPressed = false;
        }

        //Get status and coordinates
        gcStatus = psx.getGunconCoordinates (x, y);

        if (gcStatus == GUNCON_OK)
          debugln (F("STATUS: GUNCON_OK!"));
        else if (gcStatus == GUNCON_UNEXPECTED_LIGHT)
          debugln (F("STATUS: GUNCON_UNEXPECTED_LIGHT!"));
        else if (gcStatus == GUNCON_NO_LIGHT)
          debugln (F("STATUS: GUNCON_NO_LIGHT!"));
        else
          debugln (F("STATUS: GUNCON_OTHER_ERROR!"));


        if (gcStatus == GUNCON_OK) {
          lastX = x;
          lastY = y;

          //Sets min and max detected values if needed
          if (x < minX && x > 70)
            minX = x;
          else if (x > maxX && x < 470)
            maxX = x;
            
          if (y < minY && y > 20)
            minY = y;
          else if (y > maxY && y < 300)
            maxY = y;

          debug (F(" analog: x = "));
          debug (x);
          debug (F(", y = "));
          debug (y);
  
          debug (F(" MIN: x = "));
          debug (minX);
          debug (F(", y = "));
          debug (minY);
  
          debug (F(" MAX: x = "));
          debug (maxX);
          debug (F(", y = "));
          debugln (maxY);


          if(enableMouseMove)
            AbsMouse.move(convertRange(minX, maxX, x), convertRange(minY, maxY, y));
        }
        
			}
		}
	}
}
