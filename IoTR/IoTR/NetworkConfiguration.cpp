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

#include "NetworkConfiguration.h"

// https://arduinojson.org/v6/doc/upgrade/

/* @brief Singelton device configuration instance. */
NetworkConfiguration_t NetworkConfiguration;

/** @brief Load configuration.
 *  @param fileSystem FS, File system of the device.
 *  @return boolean, Successful loading.
 */
bool load_network_configuration(FS* fileSystem, const char* path) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	File file = fileSystem->open(path, "r");
	if (!file) {
		DEBUGLOG("Failed to open file.\r\n");
		set_default_network_configuration();
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

	DynamicJsonDocument  doc(1024);
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

	NetworkConfiguration.SSID = doc["SSID"].as<String>();

	NetworkConfiguration.Password = doc["Password"].as<const char *>();

	NetworkConfiguration.IP = IPAddress(doc["IP"][0], doc["IP"][1], doc["IP"][2], doc["IP"][3]);
	NetworkConfiguration.NetMask = IPAddress(doc["NetMask"][0], doc["NetMask"][1], doc["NetMask"][2], doc["NetMask"][3]);
	NetworkConfiguration.Gateway = IPAddress(doc["Gateway"][0], doc["Gateway"][1], doc["Gateway"][2], doc["Gateway"][3]);
	NetworkConfiguration.DNS = IPAddress(doc["DNS"][0], doc["DNS"][1], doc["DNS"][2], doc["DNS"][3]);

	NetworkConfiguration.DHCP = doc["DHCP"].as<bool>();
	
	return true;
}

/** @brief Save configuration.
 *  @param fileSystem FS, File system of the device.
 *  @return boolean, Successful saving.
 */
bool save_network_configuration(FS* fileSystem, const char* path) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	//flag_config = false;
	DynamicJsonDocument doc(512);

	//StaticJsonBuffer<1024> doc;
	doc["SSID"] = NetworkConfiguration.SSID;
	doc["Password"] = NetworkConfiguration.Password;

	JsonArray jsonip = doc.createNestedArray("IP");
	jsonip.add(NetworkConfiguration.IP[0]);
	jsonip.add(NetworkConfiguration.IP[1]);
	jsonip.add(NetworkConfiguration.IP[2]);
	jsonip.add(NetworkConfiguration.IP[3]);

	JsonArray jsonNM = doc.createNestedArray("NetMask");
	jsonNM.add(NetworkConfiguration.NetMask[0]);
	jsonNM.add(NetworkConfiguration.NetMask[1]);
	jsonNM.add(NetworkConfiguration.NetMask[2]);
	jsonNM.add(NetworkConfiguration.NetMask[3]);

	JsonArray jsonGateway = doc.createNestedArray("Gateway");
	jsonGateway.add(NetworkConfiguration.Gateway[0]);
	jsonGateway.add(NetworkConfiguration.Gateway[1]);
	jsonGateway.add(NetworkConfiguration.Gateway[2]);
	jsonGateway.add(NetworkConfiguration.Gateway[3]);

	JsonArray jsondns = doc.createNestedArray("DNS");
	jsondns.add(NetworkConfiguration.DNS[0]);
	jsondns.add(NetworkConfiguration.DNS[1]);
	jsondns.add(NetworkConfiguration.DNS[2]);
	jsondns.add(NetworkConfiguration.DNS[3]);

	doc["DHCP"] = NetworkConfiguration.DHCP;

	File file = fileSystem->open(path, "w");
	if (!file)
	{
		DEBUGLOG("Failed to open config file for writing\r\n");
		file.close();
		return false;
	}

#ifdef SHOW_CONFIG
	String temp;
	serializeJson(doc, temp);
	DEBUGLOG("%s\r\n", temp.c_str());
#endif

	serializeJson(doc, file);
	file.flush();
	file.close();

	return true;
}

/** @brief Default configuration.
 *  @return Void
 */
void set_default_network_configuration() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	NetworkConfiguration.SSID = DEFAULT_STA_SSID;
	NetworkConfiguration.Password = DEFAULT_STA_PASSWORD;
	NetworkConfiguration.DHCP = DEFAULT_ENABLED_DHCP;
	NetworkConfiguration.IP = DEAFULUT_IP;
	NetworkConfiguration.NetMask = DEAFULT_NET_MASK;
	NetworkConfiguration.Gateway = DEAFULT_GW;
	NetworkConfiguration.DNS = DEAFULT_DNS;
}
