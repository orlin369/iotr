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

// MqttConfiguration.h

#ifndef _MQTTAUTHENTICATIONCONFIGURATION_h
#define _MQTTAUTHENTICATIONCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Definitons

#ifndef CONFIG_MQTT
#define CONFIG_MQTT "/mqtt.json"
#endif // !CONFIG_MQTT

#pragma endregion

#pragma region Headers

#include "ApplicationConfiguration.h"

#include "DebugPort.h"

#include <FS.h>

#include <ArduinoJson.h>

#pragma endregion

#pragma region Structures

/** @brief HTTP Authentication configuration structure.
 *  @author Orlin Dimitrov <orlin369@gmail.com>
 *  @version 1.0
 *
 *  HTTP Authentication configuration structure, it hold configuration parameters values.
 */
typedef struct {
	bool Auth = true; ///< Enable - Set default to be enabled.
	int Port = DEFUALT_MQTT_PORT; ///< Service port.
	String Domain = DEFAULT_MQTT_DOMAIN; ///< User name - WWW user name. Set default value.
	String Username = DEFAULT_MQTT_USER; ///< User name - WWW user name. Set default value.
	String Password = DEFAULT_MQTT_PASS; ///< Password - WWW password. Set default value.

} MqttConfiguration_t;

#pragma endregion

#pragma region Prototypes

/** @brief Load MQTT configuration.
 *  @return boolean, Successful loading.
 */
bool load_mqtt_configuration(FS* fileSystem, const char* path);

/** @brief Save MQTT configuration.
 *  @return boolean, Return true if successful.
 */
bool save_mqtt_configuration(FS* fileSystem, const char* path);

/** @brief Default configuration.
 *  @return Void
 */
bool set_default_mqtt_configuration();

#pragma endregion

/* @brief Singelton HTTP Authentication instance. */
extern MqttConfiguration_t MqttConfiguration;

#endif
