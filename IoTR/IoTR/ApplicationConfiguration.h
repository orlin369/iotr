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

// ApplicationConfiguration.h

#ifndef _APPLICATIONCONFIGURATION_h
#define _APPLICATIONCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define NODEMCU1

#ifdef NODEMCU1

#define PIN_LED1 D3  // D3 - NodeMCU
#define PIN_LED2 D2 // D2 - NodeMCU
#define PIN_LED3 D1  // D1 - NodeMCU
#define PIN_LED4 D0  // D0 - NodeMCU

#define PIN_BTN1 D5 // D5 - NodeMCU
#define PIN_BTN2 D6 // D6 - NodeMCU
#define PIN_BTN3 D7 // D7 - NodeMCU
#define PIN_BTN4 D4  // D4 - NodeMCU

#else

#define PIN_LED1 0  // D3 - NodeMCU
#define PIN_LED2 4 // D2 - NodeMCU
#define PIN_LED3 5  // D1 - NodeMCU
#define PIN_LED4 20  // D0 - NodeMCU

#define PIN_BTN1 16 // D5 - NodeMCU
#define PIN_BTN2 12 // D6 - NodeMCU
#define PIN_BTN3 13 // D7 - NodeMCU
#define PIN_BTN4 4  // D4 - NodeMCU

#endif // NODEMCU1

#pragma region General Configuration

/** @brief Brand name. */
#define DEVICE_BRAND "IoTR"

// NOTE: Be careful with the Version number in the server and here in the source code.
// TODO: Version Controll - Coment this version notation with the team.
// 1000
// 10 - Major
// 00 - Minor
/** @brief Firmware version string. */
#define ESP_FW_VERSION 1

#define HEARTBEAT_TIME 5000

/** @brief Switch the logging terminal. */
#define DEBUGLOG SERIALLOG

/** @brief Show functions names. */
#define SHOW_FUNC_NAMES

/** @brief Show configuration file. */
#define SHOW_CONFIG

/** @brief Enable Arduino updates. */
#define ENABLE_ARDUINO_OTA

/** @brief Enable HTTP updates. */
#define ENABLE_HTTP_OTA

/** @brief Enable IR controll interface. */
#define ENABLE_IR_INTERFACE

/** @brief Enable Status LED. */
#define ENABLE_STATUS_LED

/** @brief Enable device control. */
#define ENABLE_DEVICE_CONTROL

/** @brief Enable software ON and OFF. */
#define EANBLE_SOFTWARE_ONOFF

#pragma endregion

#pragma region Device Conguration

/** @brief Default STA SSID. */
#define DEAFULT_USER "admin"

/** @brief Default STA password. */
#define DEAFULT_PASS "admin"

/** @brief Default device audrate. */
#define DEVICE_BAUDRATE 115200

#define DEFAULT_NTP_DOMAIN "europe.pool.ntp.org"

#define DEFAULT_NTP_PORT 1337

#define DEFAULT_NTP_TIMEZONE 7200

#define NTP_UPDATE_INTERVAL 60000UL

#pragma endregion

#pragma region Debug Terminal Configuration

/** @brief Debug output port. */
#define DBG_OUTPUT_PORT Serial1 // Serial1 // on D4

/** @brief Debug output port baud rate. */
#define DBG_OUTPUT_PORT_BAUDRATE 115200

#pragma endregion

#pragma region Internal WEB Server Configuration

/** @brief WEB server port. */
#define WEB_SERVER_PORT 80

/** @brief Enable editor WEB editor. */
#define ENABLE_EDITOR

/** @brief Stay connected time to the WEB server. */
#define ALIVE_TIME 300 // 300s -> 5min

#pragma endregion

#pragma region AP Configuration

/** @brief Default AP password. */
#define DEFAULT_AP_PASSWORD "12345678"

#pragma endregion

#pragma region STA Configuration

/** @brief Maximum time to connect to local AP. */
#define TIMEOUT_TO_CONNECT 10

/** @brief Default STA SSID. */
#define DEFAULT_STA_SSID "YOUR_DEFAULT_WIFI_PASSWD"

/** @brief Default STA password. */
#define DEFAULT_STA_PASSWORD "YOUR_DEFAULT_WIFI_SSID"

/** @brief Default enabled DHCP client. */
#define DEFAULT_ENABLED_DHCP 1

/** @brief Default IP address of the station. */
#define DEAFULUT_IP IPAddress(192, 168, 1, 4)

/** @brief Default mask of the station. */
#define DEAFULT_NET_MASK IPAddress(255, 255, 255, 0)

/** @brief Default gateway of the station. */
#define DEAFULT_GW IPAddress(192, 168, 1, 1)

/** @brief Default DNS of the station. */
#define DEAFULT_DNS IPAddress(192, 168, 1, 1)

#pragma endregion

#pragma region MQTT Configuration

#define DEFUALT_MQTT_PORT 1883
#define DEFAULT_MQTT_DOMAIN "broker.mqtt-dashboard.com"
#define DEFAULT_MQTT_USER ""
#define DEFAULT_MQTT_PASS ""

/*
#define ID "7b1309be-6233-11ea-bc55-0242ac130003"

#define STAT_BAT "/api/v1/robots/" + ID + "/status/battery"

7b1309be-6233-11ea-bc55-0242ac130003
Статус - /robots/{{уникален номер на робота}}/status/{{статус}}
Команда - /robots/{{уникален номер на робота}}/command/{{команда}}

ПРИМЕР:
Батерия - /robots/{{уникален номер на робота}}/ status /battery
Сензор за следене на линия - /robots/{{уникален номер на робота}}/status/ls
Ляв енкодер - /robots/{{уникален номер на робота}}/status/left/encoder
Ляв двигател обороти - /robots/{{уникален номер на робота}}/status/left/motor/rpm
*/

#pragma endregion

#pragma region HTTP Update Configuration

/** @brief Main domain of the IoTR servers. */
//#define UPDATE_SERVER_DOMAIN "http://your.domain/"

/** @brief ESP updatest root endpoint. */
//#define UPDATE_SERVER_PATH_ESP "updater/esp/index.php"

/** @brief ESP version root endpoint. */
//#define VERSION_SERVER_PATH_ESP "updater/esp/version/index.php"

#pragma endregion

#pragma region IR Configuration

#ifdef ENABLE_IR_INTERFACE
#define PIN_IR_RECV D5
#endif // ENABLE_IR_INTERFACE

#pragma endregion

#pragma region Status LED Configuration

#ifdef ENABLE_STATUS_LED

/** @brief Status LED pin */
#define PIN_RGB_LED 15 // D8 - NodeMCU

#endif // ENABLE_STATUS_LED

#pragma endregion

#pragma region Self power Controll

#ifdef EANBLE_SOFTWARE_ONOFF
#define PIN_PON D3
#endif // EANBLE_SOFTWARE_ONOFF

#pragma endregion

#ifndef DEBUGLOG
#define DEBUGLOG(...)
#endif // !DEBUGLOG

#endif // _APPLICATIONCONFIGURATION_h
