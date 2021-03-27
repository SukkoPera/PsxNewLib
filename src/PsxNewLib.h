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
 * \file PsxNewLib.h
 * \author SukkoPera <software@sukkology.net>
 * \date 27 Jan 2020
 * \brief Playstation controller interface library for Arduino
 * 
 * Please refer to the GitHub page and wiki for any information:
 * https://github.com/SukkoPera/PsxNewLib
 */

#pragma once

#include "PsxDriver.h"
#include "PsxDriverHwSpi.h"
#include "PsxDriverBitBang.h"
#include "PsxDriverDioSoftSpi.h"
#include "PsxPublicTypes.h"
#include "PsxSingleController.h"
#include "PsxMultiController.h"
