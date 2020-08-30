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

// 
// 
// 

#include "DeviceConfiguration.h"

/* @brief Singelton HTTP Authentication instance. */
DeviceConfiguration_t DeviceConfiguration;

/** @brief Load configuration.
 *  @return boolean, Successful loading.
 */
bool load_device_config(FS* fileSystem, const char* path) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	File file = fileSystem->open(path, "r");
	if (!file) {
		DEBUGLOG("Failed to open file.\r\n");
		set_default_device_config();
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
		return false;
	}

#ifdef SHOW_CONFIG
	String temp;
	serializeJson(doc, temp);
	DEBUGLOG("%s\r\n", temp.c_str());
#endif // SHOW_CONFIG

	// Logins
	DeviceConfiguration.Username = doc["user"].as<String>();
	DeviceConfiguration.Password = doc["pass"].as<String>();
	// Device
	DeviceConfiguration.PortBaudrate = doc["port_baudrate"].as<int>();
	// NTP
	DeviceConfiguration.NTPDomain = doc["ntp_domain"].as<String>();
	DeviceConfiguration.NTPPort = doc["ntp_port"].as<int>();
	DeviceConfiguration.NTPTimezone = doc["ntp_tz"].as<int>();
	// Activation code.
	DeviceConfiguration.ActivationCode = doc["activation_code"].as<int>();

	return true;
}

/** @brief Save authentication data.
 *  @return boolean, Return true if successful.
 */
bool save_device_config(FS* fileSystem, const char* path) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	//flag_config = false;
	DynamicJsonDocument doc(256);
	// Logins
	doc["user"] = DeviceConfiguration.Username;
	doc["pass"] = DeviceConfiguration.Password;
	// Device
	doc["port_baudrate"] = DeviceConfiguration.PortBaudrate;
	// NTP
	doc["ntp_domain"] = DeviceConfiguration.NTPDomain;
	doc["ntp_port"] = DeviceConfiguration.NTPPort;
	doc["ntp_tz"] = DeviceConfiguration.NTPTimezone;
	// Activation code.
	doc["activation_code"] = DeviceConfiguration.ActivationCode;

	File file = fileSystem->open(path, "w");

	if (!file) {
		DEBUGLOG("Failed to open file for writing\r\n");
		file.close();
		return false;
	}

	String temp;
	serializeJson(doc, temp);
	DEBUGLOG("%s\r\n", temp.c_str());

	serializeJson(doc, file);
	file.flush();
	file.close();

	return true;
}

bool set_default_device_config() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// Logins
	DeviceConfiguration.Username = DEAFULT_USER;
	DeviceConfiguration.Password = DEAFULT_PASS;
	// Device
	DeviceConfiguration.PortBaudrate = DEFAULT_BAUDRATE;
	// NTP
	DeviceConfiguration.NTPDomain = DEFAULT_NTP_DOMAIN;
	DeviceConfiguration.NTPPort = DEFAULT_NTP_PORT;
	DeviceConfiguration.NTPTimezone = DEFAULT_NTP_TIMEZONE;
	// Activation Code.
	DeviceConfiguration.ActivationCode = 0;
}


