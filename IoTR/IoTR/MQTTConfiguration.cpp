/*

Robo Monitor - Robot Monitoring Device System

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

// 
// 
// 

#include "MqttConfiguration.h"

/* @brief Singelton HTTP Authentication instance. */
MqttConfiguration_t MqttConfiguration;

/** @brief Load MQTT configuration.
 *  @return boolean, Successful loading.
 */
bool load_mqtt_configuration(FS* fileSystem, const char * path) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	File file = fileSystem->open(path, "r");
	if (!file) {
		DEBUGLOG("Failed to open file.\r\n");
		set_default_mqtt_configuration();
		file.close();
		return false;
	}

	size_t size = file.size();

	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);

	// We don"t use String here because ArduinoJson library requires the input
	// buffer to be mutable. If you don"t use ArduinoJson, you may as well
	// use file.readString instead.
	file.readBytes(buf.get(), size);
	file.close();

	DynamicJsonDocument doc(256);
	DeserializationError error = deserializeJson(doc, buf.get());

	if (error) {
		DEBUGLOG("Failed to parse file.\r\n");
		set_default_mqtt_configuration();
		return false;
	}

#ifdef SHOW_CONFIG
	String temp;
	serializeJson(doc, temp);
	DEBUGLOG("%s\r\n", temp.c_str());
#endif // SHOW_CONFIG

	MqttConfiguration.Auth = doc["auth"];
	MqttConfiguration.Username = doc["user"].as<String>();
	MqttConfiguration.Password = doc["pass"].as<String>();
	MqttConfiguration.Domain = doc["domain"].as<String>();
	MqttConfiguration.Port = doc["port"].as<int>();
	return true;
}

/** @brief Save MQTT configuration.
 *  @return boolean, Return true if successful.
 */
bool save_mqtt_configuration(FS* fileSystem, const char* path) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	//flag_config = false;
	DynamicJsonDocument doc(256);

	doc["auth"] = MqttConfiguration.Auth;
	doc["user"] = MqttConfiguration.Username;
	doc["pass"] = MqttConfiguration.Password;
	doc["domain"] = MqttConfiguration.Domain;
	doc["port"] = MqttConfiguration.Port;

	File file = fileSystem->open(path, "w");

	if (!file) {
		DEBUGLOG("Failed to open file for writing\r\n");
		file.close();
		return false;
	}

#ifdef SHOW_CONFIG
	String temp;
	serializeJson(doc, temp);
	DEBUGLOG("%s\r\n", temp.c_str());
#endif // SHOW_CONFIG

	serializeJson(doc, file);
	file.flush();
	file.close();

	return true;
}

/** @brief Default configuration.
 *  @return Void
 */
bool set_default_mqtt_configuration() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	MqttConfiguration.Auth = false;
	MqttConfiguration.Username = DEFAULT_MQTT_USER;
	MqttConfiguration.Password = DEFAULT_MQTT_PASS;
	MqttConfiguration.Domain = DEFAULT_MQTT_DOMAIN;
	MqttConfiguration.Port = DEFUALT_MQTT_PORT;
	
}


