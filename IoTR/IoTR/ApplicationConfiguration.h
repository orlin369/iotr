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

// ApplicationConfiguration.h

#ifndef _APPLICATIONCONFIGURATION_h
#define _APPLICATIONCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// TODO: NTP
#define PIN_LED1 D3
#define PIN_LED2 D2
#define PIN_LED3 D1
#define PIN_LED4 D0

#define PIN_BTN1 D5
#define PIN_BTN2 D6
#define PIN_BTN3 D7
#define PIN_BTN4 D4

#pragma region General Configuration

/** @brief Brand name. */
#define DEVICE_BRAND "Robo Monitor"

// NOTE: Be careful with the Version number in the server and here in the source code.
// TODO: Version Controll - Coment this version notation with the team.
// 1000
// 10 - Major
// 00 - Minor
/** @brief Firmware version string. */
#define ESP_FW_VERSION 1

/** @brief Switch the logging terminal. */
//#define DEBUGLOG SERIALLOG

/** @brief Show functions names. */
//#define SHOW_FUNC_NAMES

/** @brief Show configuration file. */
//#define SHOW_CONFIG

/** @brief Enable Arduino updates. */
//#define ENABLE_ARDUINO_OTA

/** @brief Enable HTTP updates. */
//#define ENABLE_HTTP_OTA

/** @brief Enable IR controll interface. */
//#define ENABLE_IR_INTERFACE

/** @brief Enable Status LED. */
//#define ENABLE_STATUS_LED

/** @brief Enable device control. */
//#define ENABLE_DEVICE_CONTROL

/** @brief Enable software ON and OFF. */
//#define EANBLE_SOFTWARE_ONOFF

#pragma endregion

#pragma region Device Conguration

/** @brief Default STA SSID. */
#define DEAFULT_USER "admin"

/** @brief Default STA password. */
#define DEAFULT_PASS "admin"

/** @brief Default device audrate. */
#define DEVICE_BAUDRATE 115200

#pragma endregion

#pragma region Debug Terminal Configuration

/** @brief Debug output port. */
#define DBG_OUTPUT_PORT Serial // Serial1 // on D4

/** @brief Debug output port baud rate. */
#define DBG_OUTPUT_PORT_BAUDRATE 115200

#pragma endregion

#pragma region Internal WEB Server Configuration

/** @brief WEB server port. */
#define WEB_SERVER_PORT 80

/** @brief Enable editor WEB editor. */
#define ENABLE_EDITOR

#pragma endregion

#pragma region AP Configuration

/** @brief Default AP password. */
#define DEFAULT_AP_PASSWORD "12345678"

#pragma endregion

#pragma region STA Configuration

#define TIMEOUT_TO_CONNECT 30

/** @brief Default STA SSID. */
#define DEFAULT_STA_SSID "IOsoft@home"
//#define DEFAULT_STA_SSID "YOUR_DEFAULT_WIFI_PASSWD"

/** @brief Default STA password. */
#define DEFAULT_STA_PASSWORD "pic18f4620"
//#define DEFAULT_STA_PASSWORD "YOUR_DEFAULT_WIFI_SSID"

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

#ifdef ENABLE_HTTP_OTA
#define UPDATE_SERVER_PORT 80

/** @brief Main domain of the IoTR servers. */
#define UPDATE_SERVER_DOMAIN "http://robko01.free.bg/"

/** @brief ESP updatest root endpoint. */
#define UPDATE_SERVER_PATH_ESP "api/v1/device/fota/esp/update" // "updater/esp/index.php"

/** @brief ESP version root endpoint. */
#define VERSION_SERVER_PATH_ESP "api/v1/device/fota/esp/version" // "updater/esp/version/index.php"
#endif // !ENABLE_HTTP_OTA

#pragma endregion

#pragma region IR Configuration

#ifdef ENABLE_IR_INTERFACE
#define RECV_PIN D4
#endif // ENABLE_IR_INTERFACE

#pragma endregion

#pragma region Status LED Configuration

#ifdef ENABLE_STATUS_LED

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN_RGB_LED D8

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

//TODO:vRonoliga prava draiv.
