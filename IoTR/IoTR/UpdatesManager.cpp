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

#include "UpdatesManager.h"

/** @brief Check for ESP updates procedure.
 *  @return Void.
 */
void check_update_ESP() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	uint8 c_MACL[WL_MAC_ADDR_LENGTH];
	WiFi.macAddress(c_MACL);
	String MACL = mac2str(c_MACL);

	String BinariImageUrlL = String(UPDATE_SERVER_DOMAIN) + String(UPDATE_SERVER_PATH_ESP);
	String FwVersUrlL = String(UPDATE_SERVER_DOMAIN) + String(VERSION_SERVER_PATH_ESP);

	DEBUGLOG("Checking for firmware updates.\r\n");
	DEBUGLOG("MAC address: %s\r\n", MACL.c_str());
	DEBUGLOG("Firmware version URL: %s\r\n", FwVersUrlL.c_str());

	HTTPClient HttpClientL;

	int StatusCodeL;

	HttpClientL.begin(FwVersUrlL);
	StatusCodeL = HttpClientL.GET();

	if (StatusCodeL == 200)
	{
		String NewFwVersionL = HttpClientL.getString();
		int VersionNumberL = NewFwVersionL.toInt();

		DEBUGLOG("Current   firmware version: %d\r\n", ESP_FW_VERSION);
		DEBUGLOG("Available firmware version: %d\r\n", VersionNumberL);

		if (VersionNumberL != ESP_FW_VERSION)
		{
			DEBUGLOG("Preparing to update\r\n");
			DEBUGLOG("Binary file URL : %s\r\n", BinariImageUrlL.c_str());

			t_httpUpdate_return ResponseL = ESPhttpUpdate.update(BinariImageUrlL, String(ESP_FW_VERSION));			
			switch (ResponseL)
			{
				case HTTP_UPDATE_FAILED:
					DEBUGLOG("HTTP_UPDATE_FAILD Error (%d): %s\r\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
					break;

				case HTTP_UPDATE_NO_UPDATES:
					DEBUGLOG("HTTP_UPDATE_NO_UPDATES\r\n");
					break;

				case HTTP_UPDATE_OK:
					DEBUGLOG("HTTP_UPDATE_OK\r\n");
					break;
			}
		}
		else {
			DEBUGLOG("Already on latest version.\r\n");
		}
	}
	else {
		DEBUGLOG("Firmware version check failed, got HTTP response code %d\r\n", StatusCodeL);
	}

	HttpClientL.end();
}
