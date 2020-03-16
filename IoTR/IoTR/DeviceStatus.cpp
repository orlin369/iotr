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

	String json = "";
	json += "{";
	json += "\"ts\":" + String(DeviceStatus.Timestamp);
	json += ",\"rssi\":" + String(DeviceStatus.RSSI);
	json += ",\"ssid\":\"" + DeviceStatus.SSID + "\"";
	json += ",\"voltage\":" + String(DeviceStatus.Voltage);
	json += ",\"flags\":" + String(DeviceStatus.Flags);
	json += "}";

	//DEBUGLOG("%s\r\n", json.c_str());

	return json;
}
