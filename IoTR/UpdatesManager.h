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

// UpdatesManager.h

#ifndef _UPDATESMANAGER_h
#define _UPDATESMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Headers

#include "ApplicationConfiguration.h"

#include "DebugPort.h"

#include "GeneralHelper.h"

#define DEBUG_ESP_HTTP_UPDATE
#define DEBUG_ESP_PORT DEBUGLOG

#ifndef UPDATE_SERVER_PORT
#define UPDATE_SERVER_PORT 80
#endif // !UPDATE_SERVER_PORT

#ifndef UPDATE_SERVER_DOMAIN
/** @brief Main domain of the IoTR servers. */
#define UPDATE_SERVER_DOMAIN "http://iotr.free.bg/"
#endif // !UPDATE_SERVER_DOMAIN

#ifndef UPDATE_SERVER_PATH_ESP
/** @brief ESP updatest rout endpoint. */
#define UPDATE_SERVER_PATH_ESP "api/v1/device/fota/esp/update/fw.bin" // api/v1/device/fota/esp/update
#endif // !UPDATE_SERVER_PATH_ESP

#ifndef VERSION_SERVER_PATH_ESP
/** @brief ESP version rout endpoint. */
#define VERSION_SERVER_PATH_ESP "api/v1/device/fota/esp/version/fw.txt" // api/v1/device/fota/esp/version
#endif // !VERSION_SERVER_PATH_ESP

#ifdef ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#endif

#pragma endregion

#pragma region Prototypes

/** @brief Check for ESP updates procedure.
 *  @return Void.
 */
void check_update_ESP();

#pragma endregion

#endif
