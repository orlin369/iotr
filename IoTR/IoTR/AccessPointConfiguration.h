/*

IoTR - Robot Monitoring Device System

Copyright (C) [2020] [Orlin Dimitrov] GPLv3

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// AccessPointConfiguration.h

#ifndef _ACCESSPOINTCONFIGURATION_h
#define _ACCESSPOINTCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Headers

#include "ApplicationConfiguration.h"

#pragma endregion

#pragma region Structures

/** @brief Access point configuration structure.
 *  @author Orlin Dimitrov <orlin369@gmail.com>
 *  @version 1.0
 *
 *  Access point configuration structure, it hold configuration parameters values.
 */
typedef struct {
	String SSID; ///< APssid - ChipID is appended to this name.
	String Password = DEFAULT_AP_PASSWORD; ///< APpassword - AP Password string.
	bool Enable = false; ///< AP disabled by default
} AccessPointConfiguration_t;

#pragma endregion

/* @brief Static AP config settings. */
extern AccessPointConfiguration_t AccessPointConfiguration;

#endif

