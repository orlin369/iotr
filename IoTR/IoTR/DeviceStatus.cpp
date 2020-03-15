// 
// 
// 

#include "DeviceStatus.h"

/* @brief Singelton device stater instance. */
DeviceStatus_t DeviceStatus;

String dev_status_to_json()
{
	/*
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES
*/

//flag_config = false;
	DynamicJsonDocument doc(512);

	doc["ts"] = DeviceStatus.Timestamp;
	doc["flags"] = DeviceStatus.Flags;
	doc["voltage"] = DeviceStatus.Voltage;
	doc["ssid"] = DeviceStatus.SSID;
	doc["rssi"] = DeviceStatus.RSSI;

	String content = "";

	serializeJson(doc, content);
	//DEBUGLOG("%s\r\n", content.c_str());

	return content;
}
