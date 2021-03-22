#include <Arduino.h>

//! \name Controller Commands
//! @{

/** \brief Enter Configuration Mode
 * 
 * Command used to enter the controller configuration (also known as \a escape)
 * mode
 */
static const byte enter_config[] = {0x01, 0x43, 0x00, 0x01, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
static const byte exit_config[] = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
/* These shorter versions of enter_ and exit_config are accepted by all
 * controllers I've tested, even in analog mode, EXCEPT SCPH-1200, so let's use
 * the longer ones
 */
//~ static byte enter_config[] = {0x01, 0x43, 0x00, 0x01, 0x00};
//~ static byte exit_config[] = {0x01, 0x43, 0x00, 0x00, 0x00};

/** \brief Read Controller Type
 * 
 * Command used to read the controller type.
 * 
 * This does not seem to be 100% reliable, or at least we don't know how to tell
 * all the various controllers apart.
 */
static const byte type_read[] = {0x01, 0x45, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
static const byte set_mode[] = {0x01, 0x44, 0x00, /* enabled */ 0x01, /* locked */ 0x03, 0x00, 0x00, 0x00, 0x00};
static const byte set_pressures[] = {0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
//~ static byte enable_rumble[] = {0x01, 0x4D, 0x00, 0x00, 0x01};

/** \brief Poll all buttons
 * 
 * Command used to read the status of all buttons.
 */
static const byte poll[] = {0x01, 0x42, 0x00, 0xFF, 0xFF};

/** \brief Poll all controllers
 * 
 * Command used to read the status of all controllers when using a MultiTap.
 */
static const byte multipoll[] = {0x01, 0x42, 0x01, 0x00 /* Could be 42 */, 0x00};

//! @}
