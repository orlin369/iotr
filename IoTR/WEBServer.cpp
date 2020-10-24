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

#pragma region Headers

/* @brief Smart scale web server. */
#include "WEBServer.h"

#include <StreamString.h>

#pragma endregion

#pragma region Variables

/* @brief Singleton Smart scale server instance. */
WEBServer LocalWEBServer(WEB_SERVER_PORT);

#pragma endregion

#pragma region Public Methods

/** @brief Constructor.
 *  @param port, uint16_t WEB server port.
 *  @return WEBServer
 */
WEBServer::WEBServer(uint16_t port) : AsyncWebServer(port) {}

/** @brief Begin server.
 *  @param fs, FS file system.
 *  @return Void
 */
void WEBServer::begin(FS* fs) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_fileSystem = fs;
	
	// Initialize the routs.
	this->initRouts();

	// Q: I"ve not got this to work.
	// Q: Need some investigation.
	// A: DNS record does not update.
	MDNS.begin(NetworkConfiguration.Hostname.c_str());
	MDNS.addService("http", "tcp", WEB_SERVER_PORT);

	// Generate cookie.
	m_Cookie = genSession();

#ifdef SHOW_CONFIG
	DEBUGLOG("New cookie: %s\r\n", m_Cookie.c_str());
#endif // !SHOW_CONFIG

	// Configure and start Web server
	AsyncWebServer::begin();

	// Task to run periodic things every second.
	m_eventUpdater.attach(1.0f, &WEBServer::eventUpdateHandler, static_cast<void*>(this));

	clearAliveTime();
}

/** @brief Handle OTA process.
 *  @return Void.
 */
void WEBServer::update()
{
#ifdef ESP32

#elif defined(ESP8266)
	// Update MDNS service.
	if (WiFi.status() == WL_CONNECTED)
	{	
		MDNS.update();
	}
#endif
}

void WEBServer::displayLog(String line) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (m_webSocketEvents.count() > 0) {
		m_webSocketEvents.send(line.c_str(), ESS_LOG);
	}
}

void WEBServer::displayIRCommand(uint32_t command) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (m_webSocketEvents.count() > 0) {
		m_webSocketEvents.send(String(command).c_str(), ESS_IR_CMD);
	}
}

/** @brief Execute every one second, if attached event. Part of the API.
 *  @return Void.
 */
void WEBServer::sendDeviceState(String device_state) {
/*
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES
*/

	// Chack does have someone connected.
	if (m_webSocketEvents.count() > 0)
	{
		m_webSocketEvents.send(device_state.c_str(), ESS_DEV_STATE);
	}
}

/** @brief Updates the header data.
 *  @return Void.
 */
void WEBServer::sendDeviceStatus(String data) {
/*
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES
*/

	// Check does have someone connected.
	if (m_webSocketEvents.count() > 0)
	{
		m_webSocketEvents.send(data.c_str(), ESS_DEV_STATUS);
	}
}

/** @brief Set tare process function. Part of the API.
 *  @param callback, Start function.
 *  @return Void.
 */
void WEBServer::setCbStartDevice(void(*callback)(void)) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_callbackStartDevice = callback;
}

/** @brief Set tare process function. Part of the API.
 *  @param callback, Stop function.
 *  @return Void.
 */
void WEBServer::setCbStopDevice(void(*callback)(void)) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_callbackStopDevice = callback;
}

/** @brief Set reboot process function. Part of the API.
 *  @param callback, Stop function.
 *  @return Void.
 */
void WEBServer::setCbReboot(void(*callback)(void)) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_callbackReboot = callback;
}

#pragma endregion

#pragma region Protected Methods

/** @brief Initialize the routs.
 *  @return Void.
 */
void WEBServer::initRouts() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#pragma region Serve Static

	on(BZF_RESET_PATH, HTTP_GET, [this](AsyncWebServerRequest* request) {
		if (!this->handleFileRead(BZF_RESET_PATH, request))
		{
			request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		}
		});

	on(BZF_STYLE_MIN_PATH, HTTP_GET, [this](AsyncWebServerRequest* request) {
		if (!this->handleFileRead(BZF_STYLE_MIN_PATH, request))
		{
			request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		}
		});

	on(BZF_LOGO_PATH, HTTP_GET, [this](AsyncWebServerRequest* request) {
		if (!this->handleFileRead(BZF_LOGO_PATH, request))
		{
			request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		}
		});

	on(BZF_FAVICON_PATH, HTTP_GET, [this](AsyncWebServerRequest* request) {
		if (!this->handleFileRead(BZF_FAVICON_PATH, request))
		{
			request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		}
		});

#pragma endregion

#pragma region File editor API

#ifdef ENABLE_EDITOR

	// List directory.
	on(ROUT_EDITOR_LIST, HTTP_GET, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->handleFileList(request);
	});

	// Load editor.
	on(ROUT_EDITOR, HTTP_GET, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		if (!this->handleFileRead("/edit.html", request))
		{
			request->send(404, MIME_TYPE_PLAIN_TEXT, "File Not Found!");
		}
	});

	// Create file.
	on(ROUT_EDITOR, HTTP_PUT, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->handleFileCreate(request);
	});

	// Delete file.
	on(ROUT_EDITOR, HTTP_DELETE, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->handleFileDelete(request);
	});

	// First callback is called after the request has ended with all parsed arguments.
	// Second callback handles file uploads at that location.
	on(ROUT_EDITOR, HTTP_POST,
		[this](AsyncWebServerRequest *request)
		{
			if (!this->isLoggedin(request))
			{
				this->goToLogin(request);
				return;
			}

			request->send(200, MIME_TYPE_PLAIN_TEXT, "");
		},
		[this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
		{
			if (!this->isLoggedin(request))
			{
				this->goToLogin(request);
				return;
			}

			this->handleFileUpload(request, filename, index, data, len, final);
		});

#endif // ENABLE_EDITOR

#pragma endregion

#pragma region Home page API

	// /login
	on(ROUT_PAGE_HOME, HTTP_GET, [this](AsyncWebServerRequest* request) {
		if (this->isLoggedin(request))
		{
			AsyncWebServerResponse* response = request->beginResponse(301);
			response->addHeader("Location", ROUT_PAGE_DASHBOARD);
			response->addHeader("Cache-Control", "no-cache");
			request->send(response);
			// delete response; // Free up memory!
		}
		else
		{
			this->goToLogin(request);
		}
	});

#pragma endregion

#pragma region Login page API

	// https://rntlab.com/question/esp32-web-server-using-spiffs/
	// /login
	on(ROUT_PAGE_LOGIN, HTTP_GET, [this](AsyncWebServerRequest* request) {
		if (this->isLoggedin(request))
		{
			AsyncWebServerResponse* response = request->beginResponse(301);
			response->addHeader("Location", ROUT_PAGE_DASHBOARD);
			response->addHeader("Cache-Control", "no-cache");
			request->send(response);
		}
		else
		{			
			if (!this->handleFileRead("/login.html", request))
			{
				request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
			}
		}
	});

	// /login
	on(ROUT_PAGE_LOGIN, HTTP_POST, [this](AsyncWebServerRequest* request) {
		if (this->isLoggedin(request))
		{
			AsyncWebServerResponse* response = request->beginResponse(301);
			response->addHeader("Location", ROUT_PAGE_DASHBOARD);
			response->addHeader("Cache-Control", "no-cache");
			request->send(response);
		}
		else
		{
			this->pageSendLogin(request);
		}
	});

#pragma endregion

#pragma region Logout page API

	// /logout
	on(ROUT_PAGE_LOGOUT, HTTP_GET, [this](AsyncWebServerRequest* request) {
		if (this->isLoggedin(request))
		{
			this->goToLogin(request);
		}
	});

	// /logout
	on(ROUT_PAGE_LOGOUT, HTTP_POST, [this](AsyncWebServerRequest* request) {
		if (this->isLoggedin(request))
		{
			this->goToLogin(request);
		}
	});

#pragma endregion

#pragma region Dashboard page API

	// /dashboard
	on(ROUT_PAGE_DASHBOARD, HTTP_GET, [this](AsyncWebServerRequest *request) {

		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		if (!this->handleFileRead("/dashboard.html", request))
		{
			request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		}

		clearAliveTime();
	});

	// /dashboard
	on(ROUT_PAGE_DASHBOARD, HTTP_POST, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		clearAliveTime();
	});

#pragma endregion

#pragma region Settings page API

	// /settings
	on(ROUT_PAGE_SETTINGS, HTTP_GET, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		if (!this->handleFileRead("/settings.html", request))
		{
			request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		}

		clearAliveTime();
	});

	// /settings
	on(ROUT_PAGE_SETTINGS, HTTP_POST, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->pageSendSettings(request);

		clearAliveTime();
	});

#pragma endregion

#pragma region Network page API

	// /network
	on(ROUT_PAGE_NETWORK, HTTP_GET, [this](AsyncWebServerRequest* request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		if (!this->handleFileRead("/network.html", request))
		{
			request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		}

		clearAliveTime();
	});

	// /network
	on(ROUT_PAGE_NETWORK, HTTP_POST, [this](AsyncWebServerRequest* request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->pageSendNetwork(request);

		clearAliveTime();
	});

#pragma endregion

#pragma region MQTT page API

	// /mqtt
	on(ROUT_PAGE_MQTT, HTTP_GET, [this](AsyncWebServerRequest* request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		if (!this->handleFileRead("/mqtt.html", request))
		{
			request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		}

		clearAliveTime();
	});

	// /mqtt
	on(ROUT_PAGE_MQTT, HTTP_POST, [this](AsyncWebServerRequest* request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->pageSendMqtt(request);

		clearAliveTime();
	});

#pragma endregion

#pragma region Help page API

	// /help
	on(ROUT_PAGE_HELP, HTTP_GET, [this](AsyncWebServerRequest* request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		if (!this->handleFileRead("/help.html", request))
		{
			request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		}

		clearAliveTime();
		});

#pragma endregion

#pragma region Parameters API

	// Identifier device name.
	on(ROUT_API_ID, [this](AsyncWebServerRequest* request) {
		String values = "";
		values += NetworkConfiguration.Hostname;
		request->send(200, MIME_TYPE_PLAIN_TEXT, values);
	});

	// General values: name, version
	on(ROUT_API_GEN_CONFIG, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->apiSendGeneralConfig(request);

		clearAliveTime();
	});

	// Network values: IP, GW, DNS, MASK, ... .
	on(ROUT_API_NET_CONFIG, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->apiSendNetConfig(request);

		clearAliveTime();
	});

	// Connection state values: connection state.
	on(ROUT_API_CONN_STATE, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->apiSendConnState(request);

		clearAliveTime();
	});

	// Information values: SSID, IP, GW, DNS, Mask, MAC, ... .
	on(ROUT_API_CONN_INFO, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->apiSendConnInfo(request);

		clearAliveTime();
	});

	// Scans WiFi networks.
	on(ROUT_API_NET_SCAN, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->apiSendScannedNetworks(request);

		clearAliveTime();
	});
	
	// HTTP authentication.
	on(ROUT_API_AUTH, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->apiSendAuthCfg(request);

		clearAliveTime();
	});

	// MQTT params.
	on(ROUT_API_MQTT, [this](AsyncWebServerRequest* request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		this->apiSendMqttCfg(request);

		clearAliveTime();
	});

	// Stop device.
	on(ROUT_API_DEVICE_STOP, [this](AsyncWebServerRequest* request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		if (m_callbackStopDevice != nullptr)
		{
			m_callbackStopDevice();
		}

		request->send(200, MIME_TYPE_PLAIN_TEXT, "");

		clearAliveTime();
	});

	// Start device.
	on(ROUT_API_DEVICE_START, [this](AsyncWebServerRequest* request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		if (m_callbackStartDevice != nullptr)
		{
			m_callbackStartDevice();
		}

		request->send(200, MIME_TYPE_PLAIN_TEXT, "");

		clearAliveTime();
	});

	// Reboot the controller.
	on(ROUT_API_REBOOT, [this](AsyncWebServerRequest* request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		if (m_callbackReboot != nullptr)
		{
			m_callbackReboot();
		}

		request->send(200, MIME_TYPE_PLAIN_TEXT, "");
	});

#pragma endregion

#pragma region HTTP Uploader API

	on(ROUT_API_UPLOAD, HTTP_POST,
		[](AsyncWebServerRequest* request)
		{
			if (!request->authenticate(DeviceConfiguration.Username.c_str(), DeviceConfiguration.Password.c_str()))
			{
				request->requestAuthentication();
			}

			request->send(200, MIME_TYPE_PLAIN_TEXT, "");
		},
		[this](AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final)
		{
			if (!request->authenticate(DeviceConfiguration.Username.c_str(), DeviceConfiguration.Password.c_str()))
			{
				request->requestAuthentication();
			}

			this->handleFileUpload(request, filename, index, data, len, final);
	});

#pragma endregion

#pragma region Page not found API

	// Called when the URL is not defined here.
	// Use it to load content from SPIFFS.
	onNotFound([this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		AsyncWebServerResponse *response = request->beginResponse(200);
		response->addHeader("Connection", "close");
		if (!this->handleFileRead(request->url(), request))
		{
			DEBUGLOG("Not found: %s\r\n", request->url().c_str());
			request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		}
	});

#pragma endregion

#pragma region Event conect API

	// On event client connected.
	m_webSocketEvents.onConnect([](AsyncEventSourceClient* client) {
#ifdef SHOW_FUNC_NAMES
		DEBUGLOG("\r\n");
		DEBUGLOG(__PRETTY_FUNCTION__);
		DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

		//m_webSocketEvents.setAuthentication("admin", "admin");
		//m_webSocketEvents.setAuthentication(DeviceConfiguration.Username.c_str(), DeviceConfiguration.Password.c_str());

		DEBUGLOG("Event source client connected from %s\r\n", client->client()->remoteIP().toString().c_str());
	});

	// Add the handler.
	this->addHandler(&m_webSocketEvents);

#pragma endregion

#pragma region Configuration files API

#ifndef SHOW_CONFIG

	on(CONFIG_DEVICE, HTTP_GET, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		AsyncWebServerResponse *response = request->beginResponse(403, MIME_TYPE_PLAIN_TEXT, "Forbidden");
		response->addHeader("Connection", "close");
		request->send(response);
		// delete response; // Free up memory!
	});

	on(CONFIG_NET, HTTP_GET, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		AsyncWebServerResponse *response = request->beginResponse(403, MIME_TYPE_PLAIN_TEXT, "Forbidden");
		response->addHeader("Connection", "close");
		request->send(response);
		// delete response; // Free up memory!
	});

	on(CONFIG_MQTT, HTTP_GET, [this](AsyncWebServerRequest *request) {
		if (!this->isLoggedin(request))
		{
			this->goToLogin(request);
			return;
		}

		AsyncWebServerResponse *response = request->beginResponse(403, MIME_TYPE_PLAIN_TEXT, "Forbidden");
		response->addHeader("Connection", "close");
		request->send(response);
		// delete response; // Free up memory!
	});

#endif // SHOW_CONFIG

#pragma endregion

}

#ifdef ENABLE_EDITOR

// https://github.com/gmag11/FSBrowserNG

#ifdef ESP32
// ESP32

/** @brief Handle file list.
 *  @param request AsyncWebServerRequest, Request object.
 *  @return Void.
 */
void WEBServer::handleFileList(AsyncWebServerRequest *request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (!this->isLoggedin(request))
	{
		this->goToLogin(request);
		return;
	}

	if (!request->hasArg("dir"))
	{
		request->send(500, MIME_TYPE_PLAIN_TEXT, "BAD ARGS");
		return;
	}

	String path = request->arg("dir");
	DEBUGLOG("List Directory: %s\r\n", path.c_str());

	File root = SPIFFS.open(path);
	File file = root.openNextFile();
	String output = "[";

	while(file)
	{
		String filename = String(file.name()).substring(1);
		bool show = true;

#ifndef SHOW_CONFIG
		// Do not show secrets
		String entry_name = "/" + filename;
		show &= ((entry_name != CONFIG_DEVICE));
		show &= ((entry_name != CONFIG_MQTT));
		show &= ((entry_name != CONFIG_NET));
#endif // SHOW_CONFIG

		if(show)
		{
			if (output != "[")
			{
				output += ",";
			}
			bool isDir = false;
			output += "{\"type\":\"";
			output += (isDir) ? "dir" : "file";
			output += "\",\"name\":\"";
			output += filename;
			output += "\"}";
		}

		DEBUGLOG("File: %s, size: %s\r\n", file.name(), formatBytes(file.size()).c_str()); 
		file = root.openNextFile();
	}

	output += "]";
	DEBUGLOG("%s\r\n", output.c_str());
	request->send(200, "text/json", output);
}

#elif defined(ESP8266)
// ESP8266

/** @brief Handle file list.
 *  @param request AsyncWebServerRequest, Request object.
 *  @return Void.
 */
void WEBServer::handleFileList(AsyncWebServerRequest *request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (!this->isLoggedin(request))
	{
		this->goToLogin(request);
		return;
	}

	if (!request->hasArg("dir"))
	{
		request->send(500, MIME_TYPE_PLAIN_TEXT, "BAD ARGS");
		return;
	}

	String path = request->arg("dir");
	DEBUGLOG("List Directory: %s\r\n", path.c_str());
	Dir dir = m_fileSystem->openDir(path);
	path = String();

	String output = "[";
	while (dir.next())
	{
		File entry = dir.openFile("r");

		String filename = String(entry.name()).substring(1);
		bool show = true;

#ifndef SHOW_CONFIG
		// Do not show secrets
		String entry_name = "/" + filename;
		show &= ((entry_name != CONFIG_DEVICE));
		show &= ((entry_name != CONFIG_MQTT));
		show &= ((entry_name != CONFIG_NET));
#endif // SHOW_CONFIG

		if(show)
		{
			if (output != "[")
			{
				output += ",";
			}
			bool isDir = false;
			output += "{\"type\":\"";
			output += (isDir) ? "dir" : "file";
			output += "\",\"name\":\"";
			output += filename;
			output += "\"}";
		}
		entry.close();
	}

	output += "]";
	DEBUGLOG("%s\r\n", output.c_str());
	request->send(200, "text/json", output);
}


#endif


/** @brief Create file.
 *  @param request AsyncWebServerRequest, Request object.
 *  @return Void.
 */
void WEBServer::handleFileCreate(AsyncWebServerRequest *request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (!this->isLoggedin(request))
	{
		this->goToLogin(request);
		return;
	}

	if (request->args() == 0)
	{
		return request->send(500, MIME_TYPE_PLAIN_TEXT, "BAD ARGS");
	}

	String path = request->arg(0U);
	DEBUGLOG("handleFileCreate: %s\r\n", path.c_str());
	if (path == "/")
	{
		return request->send(500, MIME_TYPE_PLAIN_TEXT, "BAD PATH");
	}

	if (m_fileSystem->exists(path))
	{
		return request->send(500, MIME_TYPE_PLAIN_TEXT, "FILE EXISTS");
	}

	File file = m_fileSystem->open(path, "w");
	if (file)
	{
		file.close();
	}
	else
	{
		return request->send(500, MIME_TYPE_PLAIN_TEXT, "CREATE FAILED");
	}

	request->send(200, MIME_TYPE_PLAIN_TEXT, "");
	path = String(); // Remove? Useless statement?
}

/** @brief Delete file.
 *  @param request AsyncWebServerRequest, Request object.
 *  @return Void.
 */
void WEBServer::handleFileDelete(AsyncWebServerRequest *request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (!this->isLoggedin(request))
	{
		this->goToLogin(request);
		return;
	}

	if (request->args() == 0)
	{
		return request->send(500, MIME_TYPE_PLAIN_TEXT, "BAD ARGS");
	}

	String path = request->arg(0U);
	DEBUGLOG("handleFileDelete: %s\r\n", path.c_str());
	if (path == "/")
	{
		return request->send(500, MIME_TYPE_PLAIN_TEXT, "BAD PATH");
	}

	if (!m_fileSystem->exists(path))
	{
		return request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
	}

	m_fileSystem->remove(path);
	request->send(200, MIME_TYPE_PLAIN_TEXT, "");
	path = String(); // Remove? Useless statement?
}

/** @brief Upload file.
 *  @param request AsyncWebServerRequest, Request object.
 *  @param filename String, Name of the file.
 *  @param index size_t, 
 *  @param data uint8_t, Content of the file.
 *  @param len size_t, Length of the file.
 *  @param final boolean, Flag for closing multipart file operation.
 *  @return Void.
 */
void WEBServer::handleFileUpload(
	AsyncWebServerRequest *request, 
	String filename, 
	size_t index, 
	uint8_t *data, 
	size_t len, 
	bool final) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	static File fsUploadFile;
	static size_t fileSize = 0;

	// Start
	if (!index)
	{
		DEBUGLOG("Handle file upload name: %s\r\n", filename.c_str());
		if (!filename.startsWith("/")) filename = "/" + filename;
		fsUploadFile = m_fileSystem->open(filename, "w");
		DEBUGLOG("First upload part.\r\n");

	}

	// Continue
	if (fsUploadFile)
	{
		DEBUGLOG("Continue upload part. Size = %u\r\n", len);
		if (fsUploadFile.write(data, len) != len)
		{
			DEBUGLOG("Write error during upload.\r\n");
		}
		else
		{
			fileSize += len;
		}
	}

	/*for (size_t index = 0; index < len; index++) {
	if (fsUploadFile)
	fsUploadFile.write(data[index]);
	}*/

	// End
	if (final)
	{
		if (fsUploadFile)
		{
			fsUploadFile.close();
		}
		DEBUGLOG("Handle file upload size: %u\r\n", fileSize);
		fileSize = 0;
	}
}

#endif // ENABLE_EDITOR

/** @brief Read file.
 *  @param path String, File path.
 *  @param request AsyncWebServerRequest, Request object.
 *  @return Void.
 */
bool WEBServer::handleFileRead(String path, AsyncWebServerRequest* request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES


	DEBUGLOG("File: %s\r\n", path.c_str());

	bool FileFoundL = false;
	AsyncWebServerResponse* response;

	// Check PROGMEM for this file.
#ifdef USE_PROGMEM_FS

	bool PROGMEMFileL = false;

	// HTML
#ifdef _BZF_LOGIN_H
	if (path == BZF_LOGIN_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_LOGIN_MT, bzf_login, BZF_LOGIN_SIZE);
	}
#endif // _BZF_LOGIN_H
#ifdef _BZF_DASHBOARD_H
	else if (path == BZF_DASHBOARD_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_DASHBOARD_MT, bzf_dashboard, BZF_DASHBOARD_SIZE);
	}
#endif //_BZF_DASHBOARD_H
#ifdef _BZF_SETTINGS_H
	else if (path == BZF_SETTINGS_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_SETTINGS_MT, bzf_settings, BZF_SETTINGS_SIZE);
	}
#endif // _BZF_SETTINGS_H
#ifdef _BZF_NETWORK_H
	else if (path == BZF_NETWORK_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_NETWORK_MT, bzf_network, BZF_NETWORK_SIZE);
	}
#endif // _BZF_NETWORK_H
#ifdef _BZF_HELP_H
	else if (path == BZF_HELP_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_HELP_MT, bzf_help, BZF_HELP_SIZE);
	}
#endif // _BZF_HELP_H
#ifdef _BZF_MQTT_H
	else if (path == BZF_MQTT_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_MQTT_MT, bzf_mqtt, BZF_MQTT_SIZE);
	}
#endif // _BZF_MQTT_H
	// CSS
	else if (path == BZF_RESET_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_RESET_MT, bzf_reset, BZF_RESET_SIZE);
	}
	else if (path == BZF_STYLE_MIN_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_STYLE_MIN_MT, bzf_style_min, BZF_STYLE_MIN_SIZE);
	}
	// JS
	else if (path == BZF_MICROAJAX_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_MICROAJAX_MT, bzf_microajax, BZF_MICROAJAX_SIZE);
	}
	else if (path == BZF_DEV_STATUS_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_DEV_STATUS_MT, bzf_dev_status, BZF_DEV_STATUS_SIZE);
	}
	// Icon
	else if (path == BZF_FAVICON_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_FAVICON_MT, bzf_favicon, BZF_FAVICON_SIZE);
	}
	// Logo
	else if (path == BZF_LOGO_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_LOGO_MT, bzf_logo, BZF_LOGO_SIZE);
	}

#ifdef ENABLE_EDITOR
	// Editor
	else if (path == BZF_EDIT_PATH)
	{
		PROGMEMFileL = true;
		response = request->beginResponse_P(200, BZF_EDIT_MT, bzf_edit, BZF_EDIT_SIZE);
	}
#endif

	if (PROGMEMFileL)
	{
		FileFoundL = true;
		response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
		response->addHeader("Pragma", "no-cache");
		response->addHeader("Expires", "0");
		response->addHeader("Content-Encoding", "gzip");
		request->send(response);
	}

#endif // USE_PROGMEM_FS

	// Check SPIFFS for this file.
	if (!FileFoundL)
	{
		String contentType = getContentType(path, request);
		String pathWithGz = path + ".gz";
		if (m_fileSystem->exists(pathWithGz) || m_fileSystem->exists(path))
		{
			if (m_fileSystem->exists(pathWithGz))
			{
				path += ".gz";
			}

			DEBUGLOG("Content type: %s\r\n", contentType.c_str());

			response = request->beginResponse(*m_fileSystem, path, contentType);
			response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
			response->addHeader("Pragma", "no-cache");
			response->addHeader("Expires", "0");

			if (path.endsWith(".gz"))
			{
				response->addHeader("Content-Encoding", "gzip");
			}

			DEBUGLOG("File %s EXIST\r\n", path.c_str());
			// Mark as found.
			FileFoundL = true;
			request->send(response);
			// delete response; // Free up memory!
			DEBUGLOG("File %s SENT\r\n", path.c_str());

		}
	}

	return FileFoundL;
}

/** @brief Settings arguments parser. Part of the API.
 *  @param request, AsyncWebServerRequest request object.
 *  @return Void.
 */
void WEBServer::pageSendLogin(AsyncWebServerRequest* request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (request->args() > 0)
	{
		String user;
		String pass;

		for (uint8_t index = 0; index < request->args(); index++)
		{
			//DEBUGLOG("Arg %s: %s\r\n", request->argName(index).c_str(), urlDecode(request->arg(index)).c_str());

#pragma region Authentication

			// HTTP Authentication.
			if (request->argName(index) == "user") {
				user = urlDecode(request->arg(index));
				continue;
			}
			if (request->argName(index) == "password") {
				pass = urlDecode(request->arg(index));
				continue;
			}

#pragma endregion

		}

		if (DeviceConfiguration.Username == user && DeviceConfiguration.Password == pass)
		{
			AsyncWebServerResponse* response = request->beginResponse(301);
			response->addHeader("Location", ROUT_PAGE_DASHBOARD);
			response->addHeader("Cache-Control", "no-cache");
			response->addHeader("Set-Cookie", m_Cookie);
			request->send(response);
			// delete response; // Free up memory!
			return;
		}
	}

	if (!this->handleFileRead("/login.html", request))
	{
		request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		return;
	}
}

/** @brief Settings arguments parser. Part of the API.
 *  @param request, AsyncWebServerRequest request object.
 *  @return Void.
 */
void WEBServer::pageSendSettings(AsyncWebServerRequest* request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (request->args() > 0)  // Save Settings
	{
		for (uint8_t index = 0; index < request->args(); index++)
		{
			DEBUGLOG("Arg %s: %s\r\n", request->argName(index).c_str(), urlDecode(request->arg(index)).c_str());

#pragma region Authentication

			// HTTP Authentication.
			if (request->argName(index) == "user") {
				DeviceConfiguration.Username = urlDecode(request->arg(index));
				continue;
			}

			if (request->argName(index) == "password") {
				// Prevent empty password field.
				String pass = urlDecode(request->arg(index));
				if (pass != "")
				{
					DeviceConfiguration.Password = pass;
					continue;
				}
			}

#pragma endregion

#pragma region Device

			if (request->argName(index) == "baudrate") {
				DeviceConfiguration.PortBaudrate = urlDecode(request->arg(index)).toInt();
				continue;
			}

			
			if (request->argName(index) == "acativation-code") {
				DeviceConfiguration.ActivationCode = urlDecode(request->arg(index)).toInt();
				continue;
			}

#pragma endregion

#pragma region NTP

			if (request->argName(index) == "ntp-domain") {
				DeviceConfiguration.NTPDomain = urlDecode(request->arg(index));
				continue;
			}


			if (request->argName(index) == "ntp-tz") {
				DeviceConfiguration.NTPTimezone = urlDecode(request->arg(index)).toInt() * SECS_IN_HOUR;
				DEBUGLOG("TZ: %d\r\n", DeviceConfiguration.NTPTimezone);
				continue;
			}

#pragma endregion

		}

		// Save configuration.
		save_device_config(m_fileSystem, CONFIG_DEVICE);

		// Apply settings to the hardware!
		COM_PORT.end();
		COM_PORT.begin(DeviceConfiguration.PortBaudrate);
		COM_PORT.flush();
	}

	if (!this->handleFileRead("/settings.html", request))
	{
		request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		return;
	}
}

/** @brief Network arguments parser. Part of the API.
 *  @param request, AsyncWebServerRequest request object.
 *  @return Void.
 */
void WEBServer::pageSendNetwork(AsyncWebServerRequest* request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (request->args() > 0)  // Save Settings
	{
		for (uint8_t index = 0; index < request->args(); index++)
		{
			DEBUGLOG("Arg %s: %s\r\n", request->argName(index).c_str(), urlDecode(request->arg(index)).c_str());

#pragma region Network

			// https://www.guidgenerator.com/online-guid-generator.aspx
			if (request->argName(index) == "hostname") {
				NetworkConfiguration.Hostname = urlDecode(request->arg(index));
				continue;
			}

			// SSID
			if (request->argName(index) == "ssid") {
				NetworkConfiguration.SSID = urlDecode(request->arg(index));
				continue;
			}

			// Password
			if (request->argName(index) == "password") {
				String pass = urlDecode(request->arg(index));
				if (pass != "")
				{
					NetworkConfiguration.Password = pass;
				}
				continue;
			}

			// IP
			if (request->argName(index) == "ip_0") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.IP[0] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "ip_1") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.IP[1] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "ip_2") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.IP[2] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "ip_3") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.IP[3] = request->arg(index).toInt();
				}
				continue;
			}

			// Net mask
			if (request->argName(index) == "nm_0") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.NetMask[0] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "nm_1") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.NetMask[1] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "nm_2") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.NetMask[2] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "nm_3") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.NetMask[3] = request->arg(index).toInt();
				}
				continue;
			}

			// Gateway
			if (request->argName(index) == "gw_0") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.Gateway[0] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "gw_1") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.Gateway[1] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "gw_2") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.Gateway[2] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "gw_3") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.Gateway[3] = request->arg(index).toInt();
				}
				continue;
			}

			// DNS
			if (request->argName(index) == "dns_0") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.DNS[0] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "dns_1") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.DNS[1] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "dns_2") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.DNS[2] = request->arg(index).toInt();
				}
				continue;
			}
			if (request->argName(index) == "dns_3") {
				if (check_octet_range(request->arg(index))) {
					NetworkConfiguration.DNS[3] = request->arg(index).toInt();
				}
				continue;
			}

			// DHCP
			if (request->argName(index) == "dhcp") {
				NetworkConfiguration.DHCP = true;
				continue;
			}

#pragma endregion

		}

		// Save device configuration.
		save_network_configuration(m_fileSystem, CONFIG_NET);
	}

	if (!this->handleFileRead("/network.html", request))
	{
		request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		return;
	}
}

/** @brief MQTT arguments parser. Part of the API.
 *  @param request, AsyncWebServerRequest request object.
 *  @return Void.
 */
void WEBServer::pageSendMqtt(AsyncWebServerRequest* request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (request->args() > 0)  // Save Settings
	{
		for (uint8_t index = 0; index < request->args(); index++)
		{
			DEBUGLOG("Arg %s: %s\r\n", request->argName(index).c_str(), urlDecode(request->arg(index)).c_str());

			// Domain
			if (request->argName(index) == "domain") {
				MqttConfiguration.Domain = urlDecode(request->arg(index));
				continue;
			}

			// Port
			if (request->argName(index) == "port") {
				MqttConfiguration.Port = urlDecode(request->arg(index)).toInt();
				continue;
			}

			// Authentication
			if (request->argName(index) == "auth") {
				MqttConfiguration.Auth = (bool)urlDecode(request->arg(index));
				continue;
			}

			// Password
			if (request->argName(index) == "pass") {
				// Prevent empty password field.
				String pass = urlDecode(request->arg(index));
				if (pass != "")
				{
					MqttConfiguration.Password = pass;
					continue;
				}
			}

			// User
			if (request->argName(index) == "user") {
				String user = urlDecode(request->arg(index));
				if (user != "")
				{
					MqttConfiguration.Username = user;
					continue;
				}
				continue;
			}

		}

		save_mqtt_configuration(m_fileSystem, CONFIG_MQTT);
	}

	if (!this->handleFileRead("/mqtt.html", request))
	{
		request->send(404, MIME_TYPE_PLAIN_TEXT, "FileNotFound");
		return;
	}
}

/** @brief Send general configuration values. Part of the API.
 *  @param request, AsyncWebServerRequest request object.
 *  @return Void.
 */
void WEBServer::apiSendGeneralConfig(AsyncWebServerRequest* request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	String values = "";
	values += "userversion|" + String(ESP_FW_VERSION) + "|div\n";
	values += "baudrate|" + String(DeviceConfiguration.PortBaudrate) + "|select\n";
	values += "ntp-domain|" + (String)DeviceConfiguration.NTPDomain + "|input\n";
	values += "ntp-tz|" + String(DeviceConfiguration.NTPTimezone / SECS_IN_HOUR) + "|select\n";
	values += "acativation-code|" + String(DeviceConfiguration.ActivationCode) + "|input\n";

	request->send(200, MIME_TYPE_PLAIN_TEXT, values);
}

/** @brief Send network configuration values. Part of the API.
 *  @param request, AsyncWebServerRequest request object.
 *  @return Void.
 */
void WEBServer::apiSendNetConfig(AsyncWebServerRequest* request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	String values = "";

	values += "hostname|" + NetworkConfiguration.Hostname + "|input\n";
	values += "ssid|" + NetworkConfiguration.SSID + "|input\n";
	values += "ip_0|" + (String)NetworkConfiguration.IP[0] + "|input\n";
	values += "ip_1|" + (String)NetworkConfiguration.IP[1] + "|input\n";
	values += "ip_2|" + (String)NetworkConfiguration.IP[2] + "|input\n";
	values += "ip_3|" + (String)NetworkConfiguration.IP[3] + "|input\n";
	values += "nm_0|" + (String)NetworkConfiguration.NetMask[0] + "|input\n";
	values += "nm_1|" + (String)NetworkConfiguration.NetMask[1] + "|input\n";
	values += "nm_2|" + (String)NetworkConfiguration.NetMask[2] + "|input\n";
	values += "nm_3|" + (String)NetworkConfiguration.NetMask[3] + "|input\n";
	values += "gw_0|" + (String)NetworkConfiguration.Gateway[0] + "|input\n";
	values += "gw_1|" + (String)NetworkConfiguration.Gateway[1] + "|input\n";
	values += "gw_2|" + (String)NetworkConfiguration.Gateway[2] + "|input\n";
	values += "gw_3|" + (String)NetworkConfiguration.Gateway[3] + "|input\n";
	values += "dns_0|" + (String)NetworkConfiguration.DNS[0] + "|input\n";
	values += "dns_1|" + (String)NetworkConfiguration.DNS[1] + "|input\n";
	values += "dns_2|" + (String)NetworkConfiguration.DNS[2] + "|input\n";
	values += "dns_3|" + (String)NetworkConfiguration.DNS[3] + "|input\n";
	values += "dhcp|" + (String)(NetworkConfiguration.DHCP ? "checked" : "") + "|chk\n";

	request->send(200, MIME_TYPE_PLAIN_TEXT, values);
	values = "";
}

/** @brief Send connection state values. Part of the API.
 *  @param request, AsyncWebServerRequest request object.
 *  @return Void.
 */
void WEBServer::apiSendConnState(AsyncWebServerRequest* request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	String state = "N/A";
	String Networks = "";
	if (WiFi.status() == 0) state = "Idle";
	else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
	else if (WiFi.status() == 2) state = "SCAN COMPLETED";
	else if (WiFi.status() == 3) state = "CONNECTED";
	else if (WiFi.status() == 4) state = "CONNECT FAILED";
	else if (WiFi.status() == 5) state = "CONNECTION LOST";
	else if (WiFi.status() == 6) state = "DISCONNECTED";

	WiFi.scanNetworks(true);

	String values = "";
	values += "connectionstate|" + state + "|div\n";
	//values += "networks|Scanning networks ...|div\n";
	request->send(200, MIME_TYPE_PLAIN_TEXT, values);
	state = "";
	values = "";
	Networks = "";
}

/** @brief Send connection information values. Part of the API.
 *  @param request, AsyncWebServerRequest request object.
 *  @return Void.
 */
void WEBServer::apiSendConnInfo(AsyncWebServerRequest *request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES
	
	String MACL = WiFi.macAddress();
	
	String values = "";
	
	values += "x_ssid|" + (String)NetworkConfiguration.SSID + "|div\n";
	values += "x_ip|" + (String)WiFi.localIP()[0] + "." + (String)WiFi.localIP()[1] + "." + (String)WiFi.localIP()[2] + "." + (String)WiFi.localIP()[3] + "|div\n";
	values += "x_netmask|" + (String)WiFi.subnetMask()[0] + "." + (String)WiFi.subnetMask()[1] + "." + (String)WiFi.subnetMask()[2] + "." + (String)WiFi.subnetMask()[3] + "|div\n";
	values += "x_gateway|" + (String)WiFi.gatewayIP()[0] + "." + (String)WiFi.gatewayIP()[1] + "." + (String)WiFi.gatewayIP()[2] + "." + (String)WiFi.gatewayIP()[3] + "|div\n";
	values += "x_dns|" + (String)WiFi.dnsIP()[0] + "." + (String)WiFi.dnsIP()[1] + "." + (String)WiFi.dnsIP()[2] + "." + (String)WiFi.dnsIP()[3] + "|div\n";
	values += "x_mac|" + MACL + "|div\n";
	values += "x_rssi|" + String(WiFi.RSSI()) + "|div\n";

	AsyncResponseStream *response = request->beginResponseStream("text/html");
	// Add header.
	response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
	response->addHeader("Pragma", "no-cache");
	response->addHeader("Expires", "0");
	// Add body.
	response->print(values);

	request->send(response);
	//delete &values;
	values = "";
}

/** @brief Send networks nearby. Part of the API.
 *  @param request, AsyncWebServerRequest request object.
 *  @return Void.
 */
void WEBServer::apiSendScannedNetworks(AsyncWebServerRequest* request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	String json = "[";
	
	int n = WiFi.scanComplete();

	if (n == WIFI_SCAN_FAILED) {
		WiFi.scanNetworks(true);
	}
	else if (n) {
		for (int index = 0; index < n; ++index) {
			if (index) json += ",";
			json += "{";
			json += "\"rssi\":" + String(WiFi.RSSI(index));
			json += ",\"ssid\":\"" + WiFi.SSID(index) + "\"";
			json += ",\"bssid\":\"" + WiFi.BSSIDstr(index) + "\"";
			json += ",\"channel\":" + String(WiFi.channel(index));
			json += ",\"secure\":" + String(WiFi.encryptionType(index));
#ifdef ESP32
// ESP32
			json += ",\"hidden\":false";
#elif defined(ESP8266)
// ESP8266
			json += ",\"hidden\":" + String(WiFi.isHidden(index) ? "true" : "false");
#endif
			json += "}";
		}
		WiFi.scanDelete();
		if (WiFi.scanComplete() == WIFI_SCAN_FAILED) {
			WiFi.scanNetworks(true);
		}
	}
	json += "]";
	request->send(200, "text/json", json);
	json = "";
}

/** @brief Send authentication configuration values. Part of the API.
 *  @param request, AsyncWebServerRequest request object.
 *  @return Void.
 */
void WEBServer::apiSendAuthCfg(AsyncWebServerRequest *request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	String values = "";

	values += "user|" + (String)DeviceConfiguration.Username + "|input\n";
	// values += "password|" + (String)DeviceConfiguration.Password + "|input\n";

	request->send(200, MIME_TYPE_PLAIN_TEXT, values);
}

/** @brief Send MQTT configuration values. Part of the API.
 *  @param request, AsyncWebServerRequest request object.
 *  @return Void.
 */
void WEBServer::apiSendMqttCfg(AsyncWebServerRequest* request) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	String values = "";

	values += "auth|" + (String)(MqttConfiguration.Auth ? "checked" : "") + "|chk\n";
	values += "user|" + (String)MqttConfiguration.Username + "|input\n";
	values += "domain|" + (String)MqttConfiguration.Domain + "|input\n";
	values += "port|" + (String)MqttConfiguration.Port + "|input\n";

	request->send(200, MIME_TYPE_PLAIN_TEXT, values);
}

/** @brief Check the authorization status.
 *  @param request, AsyncWebServer Request request object.
 *  @return boolean, True have to authenticate.
 */
bool WEBServer::isLoggedin(AsyncWebServerRequest *request) {
	if (request->hasHeader("Cookie"))
	{
		String cookie = request->header("Cookie");
		//DEBUGLOG("Found cookie: %s\r\n", cookie.c_str());
		if (cookie.indexOf(m_Cookie) != -1)
		{
			//DEBUGLOG("Authentification Successful\r\n");
			return true;
		}
	}

	//DEBUGLOG("Authentification Failed\r\n");
	return false;
}

/** @brief Redirect to login page.
 *  @param request, AsyncWebServer Request request object.
 *  @return Void.
 */
void WEBServer::goToLogin(AsyncWebServerRequest* request) {
	AsyncWebServerResponse* response = request->beginResponse(301);
	response->addHeader("Location", ROUT_PAGE_LOGIN);
	response->addHeader("Cache-Control", "no-cache");
	response->addHeader("Set-Cookie", "");
	request->send(response);
	m_Cookie = genSession();
}

/** @brief Clear keep alive counter.
 *  @return Void.
 */
void WEBServer::clearAliveTime()
{
	m_keepAliveTime = 0;
}

/** @brief Template processor.
 *  @param var, const String& NAme of the template.
 *  @return String, Template content.
 */
//String WEBServer::templateProcessor(const String& var)
//{
//	if (var == "DEVICE_BRAND")
//	{
//		DEBUGLOG("`DEVICE_BRAND\r\n");
//		return String(DEVICE_BRAND);
//	}
//
//	return String();
//}

/** @brief Execute every one second.
 *  @param arg, void Execution arguments.
 *  @return Void.
 */
void WEBServer::eventUpdateHandler(void* arg) {
	// Get instance.
	WEBServer* self = reinterpret_cast<WEBServer*>(arg);

	// Keep alive.
	self->m_keepAliveTime++;
	if (self->m_keepAliveTime > ALIVE_TIME)
	{
		self->clearAliveTime();
		self->m_Cookie = self->genSession();
	}

	// TODO: Clean old connections.
	// self->m_webSocketEvents.cleanClients();
}

#pragma endregion

#pragma region Static Functions

/** @brief Convert file extension to content type.
 *  @param filename, String Name of the file.
 *  @param request, AsyncWebServerRequest Request object.
 *  @return String, Content type.
 */
String WEBServer::getContentType(String filename, AsyncWebServerRequest *request) {

	if (request->hasArg("download")) return "application/octet-stream";
	else if (filename.endsWith(".htm")) return "text/html";
	else if (filename.endsWith(".html")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".json")) return "application/json";
	else if (filename.endsWith(".png")) return "image/png";
	else if (filename.endsWith(".gif")) return "image/gif";
	else if (filename.endsWith(".jpg")) return "image/jpeg";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	else if (filename.endsWith(".xml")) return "text/xml";
	else if (filename.endsWith(".pdf")) return "application/x-pdf";
	else if (filename.endsWith(".zip")) return "application/x-zip";
	else if (filename.endsWith(".gz")) return "application/x-gzip";
	return MIME_TYPE_PLAIN_TEXT;
}

/** @brief Generate cookie.
 *  @return String, SHA1 cookie.
 */
String WEBServer::genSession() {
	int bla = random(1111000, 99999999);
	String esper = String(bla);

#ifdef SHOW_CONFIG
	DEBUGLOG("Test SHA1 Int to String radnom: %s\r\n", esper.c_str());
#endif // SHOW_CONFIG

	return sha1(esper);
}

/** @brief Decode URL unification. Based on https://code.google.com/p/avr-netino/
 *  @param input String, String to decode.
 *  @return String, Returns the string of unified URL string.
 */
String WEBServer::urlDecode(String input) {

	char SymbolL;
	String OutputL = "";

	for (byte index = 0; index < input.length(); index++)
	{
		SymbolL = input[index];

		if (SymbolL == '+')
		{
			SymbolL = ' ';
		}

		if (SymbolL == '%')
		{
			index++;
			SymbolL = input[index];
			index++;
			SymbolL = (hex2dec(SymbolL) << 4) | hex2dec(input[index]);
		}

		OutputL.concat(SymbolL);
	}

	return OutputL;
}

#pragma endregion
