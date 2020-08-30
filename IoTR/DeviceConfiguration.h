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

// DeviceConfiguration.h

#ifndef _DEVICECONFIGURATION_H
#define _DEVICECONFIGURATION_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Definitions

#ifndef CONFIG_DEVICE
/* @brief Device configuration file. */
#define CONFIG_DEVICE "/config.json"
#endif // !CONFIG_DEVICE

#pragma endregion

#pragma region Headers

#include "ApplicationConfiguration.h"

#include "DebugPort.h"

#include <FS.h>

#include <ArduinoJson.h>

#pragma endregion

#pragma region Structures

/** @brief Device configuration structure.
 *  @author Orlin Dimitrov <orlin369@gmail.com>
 *  @version 1.0
 *
 *  Device configuration structure, it hold configuration parameters values.
 */
typedef struct {
	String Username = DEAFULT_USER; ///< User name. Set default value.
	String Password = DEAFULT_PASS; ///< Password. Set default value.
	int PortBaudrate = DEFAULT_BAUDRATE; ///< Remote device baudrate.
	String NTPDomain = DEFAULT_NTP_DOMAIN; ///< NTP Domain.
	int NTPPort = DEFAULT_NTP_PORT; ///< NTP Port.
	int NTPTimezone = DEFAULT_NTP_TIMEZONE * SECS_IN_HOUR; ///< NTP Timezone offset.
	int ActivationCode = 0; ///< Activation Code.
} DeviceConfiguration_t;

#pragma endregion

#pragma region Prototypes

/** @brief Load HTTP authentication.
 *  @return boolean, Successful loading.
 */
bool load_device_config(FS* fileSystem, const char* path);

/** @brief Save authentication data.
 *  @return boolean, Return true if successful.
 */
bool save_device_config(FS* fileSystem, const char* path);

bool set_default_device_config();

#pragma endregion

/* @brief Singelton HTTP Authentication instance. */
extern DeviceConfiguration_t DeviceConfiguration;

#endif // _DEVICECONFIGURATION_H

