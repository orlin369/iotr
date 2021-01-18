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

#include "GeneralHelper.h"

/** @brief Get MAC address.
 *  @return String, Returns the string of MAC address.
 */
String mac2str(const uint8_t * mac) {

	char MACStrL[WL_MAC_ADDR_LENGTH * 3] = { 0 };

	sprintf(MACStrL, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return  String(MACStrL);
}

/** @brief Convert a single hex digit character to its integer value. Based on https://code.google.com/p/avr-netino/
 *  @return String, Returns the string of unified URL string.
 */
unsigned char hex2dec(char c) {

	if (c >= '0' && c <= '9') {
		return((unsigned char)c - '0');
	}
	if (c >= 'a' && c <= 'f') {
		return((unsigned char)c - 'a' + 10);
	}
	if (c >= 'A' && c <= 'F') {
		return((unsigned char)c - 'A' + 10);
	}
	return(0);
}

/** @brief Converts binary array to heximal string.
 *  @param uint8_t * input, Binary input.
 *  @param unsigned int input_size, Binary input size.
 *  @param char * output, Output result.
 *  @return boolean, Returns the true if value is in the range.
 */
void bin_to_strhex(uint8_t *input, unsigned int input_size, uint8_t *output)
{
	for (unsigned int index = 0; index < input_size; index++)
	{
		output[index * 2 + 0] = hex2dec((input[index] >> 4) & 0x0F);
		output[index * 2 + 1] = hex2dec((input[index]) & 0x0F);
	}
}

/** @brief Check the Values is between: [0 - 255].
 *  @param value String, Value of the octet.
 *  @return boolean, Returns the true if value is in the range.
 */
boolean check_octet_range(String value) {
	return check_octet_range(value.toInt());
}

/** @brief Check the Values is between: [0 - 255].
 *  @param value int, Value of the octet.
 *  @return boolean, Returns the true if value is in the range.
 */
boolean check_octet_range(int value) {
	return (value > 0 && value < 256);
}

/** @brief Convert size to nice text type.
 *  @param bytes, size_t Size of the file.
 *  @return String, Nice formated size text.
 */
String formatBytes(size_t bytes) {
	if (bytes < 1024UL) {
		return String(bytes) + "B";
	}
	else if (bytes < (1048576UL)) {
		return String(bytes / 1024UL) + "KB";
	}
	else if (bytes < (1073741824UL)) {
		return String(bytes / 1048576UL) + "MB";
	}
	else {
		return String(bytes / 1073741824UL) + "GB";
	}
}

/** @brief Convert many minutes to ending minutes.
 *  @param int minutes, Minutest.
 *  @return int, Ending minutes.
 */
int to_minutes(int minutes)
{
	return minutes % 60;
}

/** @brief Convert many minutes to hours.
 *  @param int minutes, Minutest.
 *  @return int, Hours.
 */
int to_hours(int minutes)
{
	return minutes / 60;
}

/** @brief Printout in the debug console flash state.
 *  @return Void.
 */
void show_device_properties() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ESP8266)
// ESP8266
	DEBUGLOG("Flash chip size: %u\r\n", ESP.getFlashChipRealSize());
#endif

	DEBUGLOG("Sketch size: %u\r\n", ESP.getSketchSize());
	DEBUGLOG("Free flash space: %u\r\n", ESP.getFreeSketchSpace());
	DEBUGLOG("Free heap: %d\r\n", ESP.getFreeHeap());
	DEBUGLOG("Firmware version: %d\r\n", ESP_FW_VERSION);
	DEBUGLOG("SDK version: %s\r\n", ESP.getSdkVersion());
	DEBUGLOG("MAC address: %s\r\n", WiFi.macAddress().c_str());
	DEBUGLOG("\r\n");
}

#ifdef ESP32

#ifdef BATT_MONITOR

/** @brief Read battery voltage.
 *  @return float voltage.
 */
float battery_voltage(int pin)
{
	static float batt_voltage = 0.0;

	float batt_voltage = (float)(analogRead(pin)/4095)*2*3.3*1.1;

	return batt_voltage;
}

#endif

#endif
