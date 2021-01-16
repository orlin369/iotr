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

// WEBServer.h

#ifndef _WEBSERVER_h
#define _WEBSERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Definitions

#define ESS_LOG "log"
#define ESS_IR_CMD "irCommand"
#define ESS_DEV_STATUS "deviceStatus"

#define ROUT_EDITOR "/edit"
#define ROUT_EDITOR_LIST "/list"

#define ROUT_PAGE_HOME "/"
#define ROUT_PAGE_LOGIN "/login"
#define ROUT_PAGE_DASHBOARD "/dashboard"
#define ROUT_PAGE_APP "/app"
#define ROUT_PAGE_SETTINGS "/settings"
#define ROUT_PAGE_NETWORK "/network"
#define ROUT_PAGE_MQTT "/mqtt"
#define ROUT_PAGE_HELP "/help"
#define ROUT_PAGE_LOGOUT "/logout"

#define ROUT_API_ID "/api/v1/identify"
#define ROUT_API_GEN_CONFIG "/api/v1/generalcfg"
#define ROUT_API_NET_CONFIG "/api/v1/netcfg"
#define ROUT_API_CONN_STATE "/api/v1/connstate"
#define ROUT_API_CONN_INFO "/api/v1/conninfo"
#define ROUT_API_NET_SCAN "/api/v1/scan"
#define ROUT_API_AUTH "/api/v1/auth"
#define ROUT_API_MQTT "/api/v1/mqtt"
#define ROUT_API_REBOOT "/api/v1/reboot"
#define ROUT_API_UPLOAD "/api/v1/upload"
#define ROUT_API_EVENTS "/api/v1/events"

#define MIME_TYPE_PLAIN_TEXT "text/plain"

#pragma endregion

#pragma region Headers

#include "ApplicationConfiguration.h"

#ifdef ESP32
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <SPIFFS.h>
#include <Hash.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#endif

#include <FS.h>
#include <Ticker.h>

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "NetworkConfiguration.h"

#include "AccessPointConfiguration.h"

#include "DeviceConfiguration.h"

#include "MQTTConfiguration.h"

#include "GeneralHelper.h"

#ifdef USE_PROGMEM_FS
#include "pages\bzf_dashboard.h"
#include "pages\bzf_app.h"
#include "pages\bzf_dev_status.h"
#include "pages\bzf_edit.h"
#include "pages\bzf_favicon.h"
#include "pages\bzf_help.h"
#include "pages\bzf_login.h"
#include "pages\bzf_logo_white.h"
#include "pages\bzf_logo.h"
#include "pages\bzf_microajax.h"
#include "pages\bzf_mqtt.h"
#include "pages\bzf_network.h"
#include "pages\bzf_reset.h"
#include "pages\bzf_settings.h"
#include "pages\bzf_style.h"
#endif // USE_PROGMEM_FS

#pragma endregion

class WEBServer : public AsyncWebServer {

public:

#pragma region Methods

	/** @brief Constructor.
	 *  @param port, uint16_t WEB server port.
	 *  @return WEBServer
	 */
	WEBServer(uint16_t port);

	/** @brief Begin server.
	 *  @param fs, FS file system.
	 *  @return Void
	 */
	void begin(FS* fs);

	/**
	 * @brief Handle periodic processes.
	 * 
	 */
	void update();
	
	 /** @brief Updates device status.
	  *  @param data, String JSON status.
	  *  @return Void
	  */
	void sendDeviceStatus(String status);

	/**
	 * @brief Display IR command.
	 * 
	 * @param command The command.
	 */
	void displayIRCommand(uint32_t command);

	/** @brief Set reboot process function. Part of the API.
	 *  @param callback, Reboot function.
	 *  @return Void.
	 */
	void setCbReboot(void(*callback)(void));

#pragma endregion

protected:

#pragma region Variables

	/**
	 * @brief File system object.
	 * 
	 */
	FS* m_fileSystem;

	/**
	 * @brief Cookie
	 * 
	 */
	String m_Cookie = "";

	/**
	 * @brief Seconds tick handler.
	 * 
	 */
	Ticker m_eventUpdater;

	/**
	 * @brief Events API handler.
	 * 
	 */
	AsyncEventSource m_webSocketEvents = AsyncEventSource(ROUT_API_EVENTS);

	/**
	 * @brief Keep alive time.
	 * 
	 */
	unsigned int m_keepAliveTime = 0;

	/**
	 * @brief Callback function
	 * 
	 */
	void(*m_callbackReboot)(void);

	/**
	 * @brief Callback function.
	 * 
	 */
	String(*m_callbackUpdateHeaderData)(void);

#pragma endregion

#pragma region Methods

#ifdef ENABLE_EDITOR

	/** @brief Handle file list.
	 *  @param request AsyncWebServerRequest, Request object.
	 *  @return Void.
	 */
	void handleFileList(AsyncWebServerRequest *request);

	/** @brief Create file.
	 *  @param request AsyncWebServerRequest, Request object.
	 *  @return Void.
	 */
	void handleFileCreate(AsyncWebServerRequest *request);

	/** @brief Delete file.
	 *  @param request AsyncWebServerRequest, Request object.
	 *  @return Void.
	 */
	void handleFileDelete(AsyncWebServerRequest *request);

	/** @brief Upload file.
	 *  @param request AsyncWebServerRequest, Request object.
	 *  @param filename String, Name of the file.
	 *  @param index size_t,
	 *  @param data uint8_t, Content of the file.
	 *  @param len size_t, Length of the file.
	 *  @param final boolean, Flag for closing multi part file operation.
	 *  @return Void.
	 */
	void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

#endif // ENABLE_EDITOR

	/** @brief Read file.
	 *  @param path String, File path.
	 *  @param request AsyncWebServerRequest, Request object.
	 *  @return Void.
	 */
	bool handleFileRead(String path, AsyncWebServerRequest* request);


	/** @brief Settings arguments parser. Part of the API.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void pageSendLogin(AsyncWebServerRequest* request);

	/** @brief Settings arguments parser. Part of the API.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void pageSendSettings(AsyncWebServerRequest* request);

	/** @brief Network arguments parser. Part of the API.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void pageSendNetwork(AsyncWebServerRequest* request);

	/** @brief MQTT arguments parser. Part of the API.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void pageSendMqtt(AsyncWebServerRequest* request);

	/** @brief Send general configuration values. Part of the API.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void apiSendGeneralConfig(AsyncWebServerRequest* request);

	/** @brief Send network configuration values. Part of the API.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void apiSendNetConfig(AsyncWebServerRequest* request);

	/** @brief Send connection state values. Part of the API.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void apiSendConnState(AsyncWebServerRequest* request);

	/** @brief Send connection information values. Part of the API.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void apiSendConnInfo(AsyncWebServerRequest* request);

	/** @brief Send networks nearby. Part of the API.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void apiSendScannedNetworks(AsyncWebServerRequest* request);

	/** @brief Send authentication configuration values. Part of the API.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void apiSendAuthCfg(AsyncWebServerRequest* request);

	/** @brief Send MQTT configuration values. Part of the API.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void apiSendMqttCfg(AsyncWebServerRequest* request);

	/** @brief Check the authorization status.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return boolean, True have to authenticate.
	 */
	bool isLoggedin(AsyncWebServerRequest *request);

	/** @brief Redirect to login page.
	 *  @param request, AsyncWebServerRequest request object.
	 *  @return Void.
	 */
	void goToLogin(AsyncWebServerRequest* request);

	/** @brief Initialize the server.
	 *  @return Void.
	 */
	void initRouts();

	/** @brief Clear keep alive counter.
	 *  @return Void.
	 */
	void clearAliveTime();

	/** @brief Template processor.
	 *  @param var, const String& NAme of the template.
	 *  @return String, Template content.
	 */
	//static String templateProcessor(const String& var);

	/** @brief Convert file extension to content type.
	 *  @param filename, String Name of the file.
	 *  @param request, AsyncWebServerRequest Request object.
	 *  @return String, Content type.
	 */
	static String getContentType(String filename, AsyncWebServerRequest *request);

	/** @brief Execute every one second.
	 *  @param arg, void Execution arguments.
	 *  @return Void.
	 */
	static void eventUpdateHandler(void* arg);

	/** @brief Generate cookie.
	 *  @return String, SHA1 cookie.
	 */
	static String genSession();

	/** @brief Decode URL unification. Based on https://code.google.com/p/avr-netino/
	 *  @param input String, String to decode.
	 *  @return String, Returns the string of unified URL string.
	 */
	static String urlDecode(String input);

#pragma endregion

};

/* @brief Singleton base WEB server instance. */
//extern WEBServer LocalWEBServer;

#endif

