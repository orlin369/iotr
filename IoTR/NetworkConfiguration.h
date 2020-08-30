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

// NetworkConfiguration.h

#ifndef _DEVICECONFIGURATION_h
#define _DEVICECONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Definitions

#ifndef CONFIG_NET
/* @brief Network configuration file. */
#define CONFIG_NET "/network.json"
#endif // !CONFIG_NET

#pragma endregion

#pragma region Headers

#include "ApplicationConfiguration.h"

#include "DebugPort.h"

#include <FS.h>

#include "IPAddress.h"

#include <ArduinoJson.h>

#pragma endregion

#pragma region Structures

/** @brief Device configuration structure
 *  @author Orlin Dimitrov <orlin369@gmail.com>
 *  @version 1.0
 *
 *  Device configuration structure, it hold configuration parameters values.
 */
typedef struct {
    String Hostname = DEVICE_BRAND; ///< Host name.
    String SSID; ///< SSID of the near AP.
	String Password; ///< Password of the near AP.
	IPAddress  IP; ///< Local IP address.
	IPAddress  NetMask; ///< Local network mask.
	IPAddress  Gateway; ///< Local gateway.
	IPAddress  DNS; ///< Local DNS.
	bool DHCP; ///< DHCP enable flag.
} NetworkConfiguration_t;

#pragma endregion

#pragma region Functions

/** @brief Load configuration.
 *  @param fileSystem FS, File system of the device.
 *  @return boolean, Successful loading.
 */
bool load_network_configuration(FS* fileSystem, const char* path);

/** @brief Save configuration.
 *  @param fileSystem FS, File system of the device.
 *  @return boolean, Successful saving.
 */
bool save_network_configuration(FS* fileSystem, const char* path);

/** @brief Default configuration.
 *  @return Void
 */
void set_default_network_configuration();

#pragma endregion

/* @brief Singelton device configuration instance. */
extern NetworkConfiguration_t NetworkConfiguration;

#endif

