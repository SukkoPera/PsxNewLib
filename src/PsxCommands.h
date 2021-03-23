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
 * \file PsxMultiTap.h
 * \author SukkoPera <software@sukkology.net>
 * \date 22 Mar 2021
 * \brief Playstation Controller Commands
 * 
 * Please refer to the GitHub page and wiki for any information:
 * https://github.com/SukkoPera/PsxNewLib
 */

#include <Arduino.h>

//! \name Controller Commands
//! @{

/** \brief Enter Configuration Mode
 * 
 * Command used to enter the controller configuration (also known as \a escape)
 * mode
 */
const byte enter_config[] = {0x01, 0x43, 0x00, 0x01, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
const byte exit_config[] = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
/* These shorter versions of enter_ and exit_config are accepted by all
 * controllers I've tested, even in analog mode, EXCEPT SCPH-1200, so let's use
 * the longer ones
 */
//~ byte enter_config[] = {0x01, 0x43, 0x00, 0x01, 0x00};
//~ byte exit_config[] = {0x01, 0x43, 0x00, 0x00, 0x00};

/** \brief Read Controller Type
 * 
 * Command used to read the controller type.
 * 
 * This does not seem to be 100% reliable, or at least we don't know how to tell
 * all the various controllers apart.
 */
const byte type_read[] = {0x01, 0x45, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
const byte set_mode[] = {0x01, 0x44, 0x00, /* enabled */ 0x01, /* locked */ 0x03, 0x00, 0x00, 0x00, 0x00};
const byte set_pressures[] = {0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
//~ byte enable_rumble[] = {0x01, 0x4D, 0x00, 0x00, 0x01};

/** \brief Poll all buttons
 * 
 * Command used to read the status of all buttons.
 */
const byte poll[] = {0x01, 0x42, 0x00, 0xFF, 0xFF};

/** \brief Poll all controllers
 * 
 * Command used to read the status of all controllers when using a MultiTap.
 */
const byte multipoll[] = {0x01, 0x42, 0x01, 0x00 /* Could be 42 */, 0x00};

//! @}
