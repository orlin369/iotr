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

#define ESS_DEV_STATE "deviceState"
#define ROUT_API_DEVICE_STOP "/api/v1/device/stop"
#define ROUT_API_DEVICE_START "/api/v1/device/start"

#pragma region Headers

/* Application configuration. */
#include "ApplicationConfiguration.h"

// SDK
#ifdef ESP32
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#endif

// #include <WiFiClient.h>
#include <FS.h>

#include <AsyncMqttClient.h>
#include <NTPClient.h>

/* Debug serial port. */
#include "DebugPort.h"

/* Device configuration. */
#include "NetworkConfiguration.h"

/* Base WEB server. */
#include "WEBServer.h"

#include "FxTimer.h"

#include "DeviceStatus.h"

#ifdef ENABLE_ARDUINO_OTA
#include <ArduinoOTA.h>
#endif // ENABLE_ARDUINO_OTA

#ifdef ENABLE_HTTP_OTA
#include "UpdatesManager.h"
#endif // ENABLE_HTTP_OTA

#ifdef ENABLE_IR_INTERFACE
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#endif // ENABLE_IR_INTERFACE

#ifdef ENABLE_STATUS_LED
#include "StatusLed.h"
#endif // ENABLE_STATUS_LED

#ifdef ENABLE_RESCUE_BTN
#include "RescueButton.h"
#endif // ENABLE_RESCUE_BTN

#ifdef USE_PROGMEM_FS
#include "pages\bzf_app.h"
#endif

#include "DeviceState.h"

#pragma endregion

#pragma region Classes

class AppWEBServer : public WEBServer {

protected:

	/**
	 * @brief Callback function for starting device.
	 * 
	 */
	void(*m_callbackStartDevice)(void);

	/**
	 * @brief Callback function for stopping device.
	 * 
	 */
	void(*m_callbackStopDevice)(void);

	void setAppRouts()
	{
		// Stop device.
		on(ROUT_API_DEVICE_STOP, [this](AsyncWebServerRequest* request) {
			if (!this->isLoggedin(request))
			{
				this->goToLogin(request);
				return;
			}

			if (this->m_callbackStopDevice != nullptr)
			{
				this->m_callbackStopDevice();
			}

			request->send(200, MIME_TYPE_PLAIN_TEXT, "");

			this->clearAliveTime();
		});

		// Start device.
		on(ROUT_API_DEVICE_START, [this](AsyncWebServerRequest* request) {
			if (!this->isLoggedin(request))
			{
				this->goToLogin(request);
				return;
			}

			if (this->m_callbackStartDevice != nullptr)
			{
				this->m_callbackStartDevice();
			}

			request->send(200, MIME_TYPE_PLAIN_TEXT, "");

			this->clearAliveTime();
		});

		on("/api/v1/keepalive", [this](AsyncWebServerRequest* request) {
			if (!this->isLoggedin(request))
			{
				this->goToLogin(request);
				return;
			}

			request->send(200, MIME_TYPE_PLAIN_TEXT, "");

			this->clearAliveTime();
		});

		// Start device.
		on("/api/v1/device/serial", [this](AsyncWebServerRequest* request) {
			if (!this->isLoggedin(request))
			{
				this->goToLogin(request);
				return;
			}

			if (request->args() > 0)  // Save Settings
			{
				for (uint8_t index = 0; index < request->args(); index++)
				{
					DEBUGLOG("Arg %s: %s\r\n", request->argName(index).c_str(), urlDecode(request->arg(index)).c_str());

					// HTTP Authentication.
					if (request->argName(index) == "read") {
						Serial.print(urlDecode(request->arg(index)));
						continue;
					}

					if (request->argName(index) == "write") {
						Serial.print(urlDecode(request->arg(index)));
						continue;
					}
				}
			}

			request->send(200, MIME_TYPE_PLAIN_TEXT, "");

			this->clearAliveTime();
		});

	}

public:

	/**
	 * @brief Construct a new App W E B Server object
	 * 
	 * @param port , uint16_t WEB server port.
	 */
	AppWEBServer(uint16_t port) : WEBServer(port) {}

	/**
	 * @brief Execute every one second, if attached event. Part of the API.
	 * 
	 * @param device_state Device state.
	 */
	void sendDeviceState(String device_state) {
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
			m_webSocketEvents.send(device_state.c_str(), ESS_DEV_STATE);
		}
	}

	/**
	 * @brief Initialize the application.
	 * 
	 * @param fs File System pointer.
	 */
	void init(FS* fs)
	{
		this->begin(fs);
		this->setAppRouts();
	}

	/**
	 * @brief Set the Cb Start Device object
	 * 
	 * @param callback Callback pointer.
	 */
	void setCbStartDevice(void(*callback)(void)) {
	#ifdef SHOW_FUNC_NAMES
		DEBUGLOG("\r\n");
		DEBUGLOG(__PRETTY_FUNCTION__);
		DEBUGLOG("\r\n");
	#endif // SHOW_FUNC_NAMES

		this->m_callbackStartDevice = callback;
	}

	/**
	 * @brief Set the Cb Stop Device object
	 * 
	 * @param callback Callback pointer.
	 */
	void setCbStopDevice(void(*callback)(void)) {
	#ifdef SHOW_FUNC_NAMES
		DEBUGLOG("\r\n");
		DEBUGLOG(__PRETTY_FUNCTION__);
		DEBUGLOG("\r\n");
	#endif // SHOW_FUNC_NAMES

		this->m_callbackStopDevice = callback;
	}

	/**
	 * @brief Display log line.
	 * 
	 * @param line Text of the line.
	 */
	void displayLog(String line) {
	#ifdef SHOW_FUNC_NAMES
		DEBUGLOG("\r\n");
		DEBUGLOG(__PRETTY_FUNCTION__);
		DEBUGLOG("\r\n");
	#endif // SHOW_FUNC_NAMES

		if (m_webSocketEvents.count() > 0) {
			m_webSocketEvents.send(line.c_str(), ESS_LOG);
		}
	}
};

#pragma endregion

#pragma region Variables

/** @brief NTP UDP socket. */
WiFiUDP NTP_UDP_g;

/** @brief NTP client. */
NTPClient NTPClient_g(NTP_UDP_g);

/** @brief WiFi connection timer. */
FxTimer WiFiConnTimer_g = FxTimer();

/** @brief MQTT connection timer. */
FxTimer MQTTConnTimer_g = FxTimer();

/** @brief Heart beat timer. */
FxTimer HeartbeatTimer_g = FxTimer();

/** @brief Device state timer. */
FxTimer DeviceStateTimer_g = FxTimer();

/** @brief MQTT client */
AsyncMqttClient MQTTClient_g;

#ifdef ENABLE_IR_INTERFACE

/**
 * @brief IR receiver.
 * 
 * @return IRrecv 
 */
IRrecv IRReceiver_g(PIN_IR_RECV);

/**
 * @brief IR receiver result.
 * 
 */
decode_results IRResults_g;

#endif // ENABLE_IR_INTERFACE

/** @brief Timestamp text buffer. */
char TimestampBuff_g[18];

/**
 * @brief Application WEB server.
 * 
 * @return AppWEBServer 
 */
AppWEBServer AppWEBServer_g(WEB_SERVER_PORT);

#pragma endregion

#pragma region Prototypes

/**
 * @brief Configure the file system.
 * 
 */
void configure_file_system();

// AP functions
#ifdef ESP32
// ESP32


#elif defined(ESP8266)
// ESP8266

/**
 * @brief Configure WiFi module to access point.
 * 
 */
void configure_to_ap();

/**
 * @brief Handler that execute when client is connected.
 * 
 * @param evt Callback handler
 */
void handler_ap_mode_station_connected(const WiFiEventSoftAPModeStationConnected& evt);

/**
 * @brief Handler that execute when client is disconnected.
 * 
 * @param evt Callback handler
 */
void handler_ap_mode_station_disconnected(const WiFiEventSoftAPModeStationDisconnected& evt);

#endif

/**
 * @brief Configure WiFi module to station.
 * 
 */
void configure_to_sta();

/**
 * @brief Printout in the debug console flash state.
 * 
 */
void show_device_properties();

#ifdef ENABLE_ARDUINO_OTA
/**
 * @brief Configure Arduino OTA module.
 * 
 */
void configure_arduino_ota();
#endif // ENABLE_ARDUINO_OTA

#pragma endregion

#pragma region Functions

#pragma region SOFTWARE ON OFF

/**
 * @brief Power ON the target device.
 * 
 */
void power_on() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	digitalWrite(PIN_RELAY, HIGH);
	// Publish message.
	MQTTClient_g.publish(TOPIC_RELAY_OUT, 2, true, "1");
}

/**
 * @brief Power OFF the target device.
 * 
 */
void power_off() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	digitalWrite(PIN_RELAY, LOW);
	// Publish message.
	MQTTClient_g.publish(TOPIC_RELAY_OUT, 2, true, "0");
}

#pragma endregion

#pragma region File System

/**
 * @brief Configure the file system.
 * 
 */
void configure_file_system() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	if (!SPIFFS.begin())
	{
		DEBUGLOG("Can not load file system.\r\n");

		for (;;) {
#ifdef ESP32
// ESP32

#elif defined(ESP8266)
// ESP8266
			ESP.wdtFeed();
#endif
		}
	}

#ifdef DEBUGLOG
#ifdef ESP32
// ESP32
	File root = SPIFFS.open("/");
	File file = root.openNextFile();
	while(file)
	{
		DEBUGLOG("File: %s, size: %s\r\n", file.name(), formatBytes(file.size()).c_str()); 
		file = root.openNextFile();
	}

#elif defined(ESP8266)
// ESP8266
	// List files
	Dir dir = SPIFFS.openDir("/");
	while (dir.next())
	{
		String fileName = dir.fileName();
		size_t fileSize = dir.fileSize();
		DEBUGLOG("File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
	}

#endif

	DEBUGLOG("\r\n");
#endif // DEBUGLOG
}

#pragma endregion

#pragma region AP mode

#ifdef ESP32
// ESP32

/**
 * @brief Configure to AP mode for ESP32.
 * 
 */
void configure_to_ap() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// Set the host name.
	// WiFi.hostname(NetworkConfiguration.Hostname.c_str());

	// Set the mode.
	WiFi.mode(WIFI_AP);

	uint32_t chipId = 0;
	for(int i=0; i<17; i=i+8)
	{
		chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}

	// NOTE: See description, this can be used as unique identification of the device.
	AccessPointConfiguration.SSID = String(DEVICE_BRAND) + String(" - ") + String(chipId);

	// Create AP name.
	DEBUGLOG("AP Name: %s\r\n", AccessPointConfiguration.SSID.c_str());

	if (AccessPointConfiguration.Enable) {
		WiFi.softAP(AccessPointConfiguration.SSID.c_str(), AccessPointConfiguration.Password.c_str());
		DEBUGLOG("AP Pass enabled: %s\r\n", AccessPointConfiguration.Password.c_str());
	}
	else {
		WiFi.softAP(AccessPointConfiguration.SSID.c_str());
		DEBUGLOG("AP Pass disabled.\r\n");
	}
}

#elif defined(ESP8266)
// ESP8266

/**
 * @brief Configure to AP mode for ESP8266.
 * 
 */
void configure_to_ap() {
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");

	// Register event handlers.
	// Callback functions will be called as long as these handler objects exist.
	// Call "handler_ap_mode_station_connected" each time a station connects
	WiFi.onSoftAPModeStationConnected(&handler_ap_mode_station_connected);

	// Call "handler_ap_mode_station_disconnected" each time a station disconnects
	WiFi.onSoftAPModeStationDisconnected(&handler_ap_mode_station_disconnected);

	// Set the host name.
	WiFi.hostname(NetworkConfiguration.Hostname.c_str());

	// Set the mode.
	WiFi.mode(WIFI_AP);

	// NOTE: See description, this can be used as unique identification of the device.
	AccessPointConfiguration.SSID = String(DEVICE_BRAND) + String(" - ") + (String)ESP.getChipId();

	// Create AP name.
	DEBUGLOG("AP Name: %s\r\n", AccessPointConfiguration.SSID.c_str());

	if (AccessPointConfiguration.Enable)
	{
		WiFi.softAP(AccessPointConfiguration.SSID.c_str(), AccessPointConfiguration.Password.c_str());
		DEBUGLOG("AP Pass enabled: %s\r\n", AccessPointConfiguration.Password.c_str());
	}
	else
	{
		WiFi.softAP(AccessPointConfiguration.SSID.c_str());
		DEBUGLOG("AP Pass disabled.\r\n");
	}
}

/** @brief Handler that execute when client is connected.
 *  @param const WiFiEventSoftAPModeStationConnected& evt, Callback handler
 *  @return Void.
 */
void handler_ap_mode_station_connected(const WiFiEventSoftAPModeStationConnected& evt) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Station connected: %s\r\n", mac2str(evt.mac).c_str());
}

/** @brief Handler that execute when client is disconnected.
 *  @param const WiFiEventSoftAPModeStationDisconnected& evt, Callback handler
 *  @return Void.
 */
void handler_ap_mode_station_disconnected(const WiFiEventSoftAPModeStationDisconnected& evt) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Station disconnected: %s\r\n", mac2str(evt.mac).c_str());
}

#endif

#pragma endregion

#pragma region STA mode

/**
 * @brief Configure WiFi module to station.
 * 
 */
void configure_to_sta() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// Set the host name.
#ifdef ESP32
// ESP32

#elif defined(ESP8266)
// ESP8266
	// Set the host name.
	WiFi.hostname(NetworkConfiguration.Hostname.c_str());
#endif

	// Disconnect required here,
	// improves reconnect reliability.
	WiFi.disconnect();

	// Encourage clean recovery after disconnect species5618, 08-March-2018.
	WiFi.setAutoReconnect(true);
	WiFi.mode(WIFI_STA);

	DEBUGLOG("Connecting: %s\r\n", NetworkConfiguration.SSID.c_str());

	WiFi.begin(NetworkConfiguration.SSID.c_str(), NetworkConfiguration.Password.c_str());

	// Check the DHCP.
	if (!NetworkConfiguration.DHCP)
	{
		DEBUGLOG("DHCP: Disabled\r\n");
		WiFi.config(NetworkConfiguration.IP, NetworkConfiguration.Gateway, NetworkConfiguration.NetMask, NetworkConfiguration.DNS);
	}

	WiFiConnTimer_g.setExpirationTime(TIMEOUT_TO_CONNECT * 1000UL);
	while (WiFi.status() != WL_CONNECTED)
	{ // Wait for the Wi-Fi to connect
		//DEBUGLOG("Stat: %d\r\n", WiFi.status());
		WiFiConnTimer_g.update();
		if (WiFiConnTimer_g.expired())
		{
			WiFiConnTimer_g.clear();
			configure_to_ap();
			return;
		}
		delay(500);
#ifdef ESP32
// ESP32

#elif defined(ESP8266)
// ESP8266
		ESP.wdtFeed();
#endif
	}

	DEBUGLOG("Connected:  %s\r\n", NetworkConfiguration.SSID.c_str());
	DEBUGLOG("IP Address: %s\r\n", WiFi.localIP().toString().c_str());
	DEBUGLOG("Gateway:    %s\r\n", WiFi.gatewayIP().toString().c_str());
	DEBUGLOG("DNS:        %s\r\n", WiFi.dnsIP().toString().c_str());
	DEBUGLOG("\r\n");
}

#pragma endregion

#pragma region Local WEB Server

/**
 * @brief Configure the WEB server.
 * 
 */
void configure_web_server() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	AppWEBServer_g.init(&SPIFFS);
	AppWEBServer_g.setCbReboot(reboot);
	AppWEBServer_g.setCbStopDevice(power_off);
	AppWEBServer_g.setCbStartDevice(power_on);
}

/**
 * @brief Reboot callback.
 * 
 */
void reboot() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	ESP.restart();
}

#pragma endregion

#pragma region Arduino OTA Updates

#ifdef ENABLE_ARDUINO_OTA

/**
 * @brief Configure Arduino OTA module.
 * 
 */
void configure_arduino_ota() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// Port defaults to 8266
	// ArduinoOTA.setPort(8266);

	// Host name defaults to esp8266-[ChipID]
	ArduinoOTA.setHostname(NetworkConfiguration.Hostname.c_str());

	// No authentication by default
	ArduinoOTA.setPassword(DeviceConfiguration.Password.c_str());

	// Password can be set with it's md5 value as well
	// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
	// ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH) {
			type = "sketch";
		}
		else { // U_SPIFFS
			type = "filesystem";
		}

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		DEBUGLOG("Start updating: %s\r\n", type.c_str());
		});

	ArduinoOTA.onEnd([]() {
		DEBUGLOG("End\r\n");
		});

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		DEBUGLOG("Progress: %u%%\r", (progress / (total / 100)));
		});

	ArduinoOTA.onError([](ota_error_t error) {
		DEBUGLOG("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) {
			DEBUGLOG("Auth Failed");
		}
		else if (error == OTA_BEGIN_ERROR) {
			DEBUGLOG("Begin Failed");
		}
		else if (error == OTA_CONNECT_ERROR) {
			DEBUGLOG("Connect Failed");
		}
		else if (error == OTA_RECEIVE_ERROR) {
			DEBUGLOG("Receive Failed");
		}
		else if (error == OTA_END_ERROR) {
			DEBUGLOG("End Failed");
		}
		});

	ArduinoOTA.begin();
}
#endif // ENABLE_ARDUINO_OTA

#pragma endregion

#pragma region NTP

/**
 * @brief Configure NTP module.
 * 
 */
void configure_ntp() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	NTPClient_g.setPoolServerName(DeviceConfiguration.NTPDomain.c_str());
	NTPClient_g.setTimeOffset(DeviceConfiguration.NTPTimezone);
	NTPClient_g.setUpdateInterval(DEFAULT_NTP_UPDATE_INTERVAL);
	NTPClient_g.begin(DEFAULT_NTP_PORT);
}

#pragma endregion

#pragma region MQTT Service

/**
 * @brief On MQTT Connect.
 * 
 * @param sessionPresent Session present.
 */
void onMqttConnect(bool sessionPresent) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	uint16_t PacketIdSubL;

	DEBUGLOG("Connected to MQTT.\r\n");
	DEBUGLOG("Session present: %d\r\n", sessionPresent);

	PacketIdSubL = MQTTClient_g.subscribe(TOPIC_SER_OUT, 2);
	DEBUGLOG("Subscribing at QoS 2, packetId: %d\r\n", PacketIdSubL);

	PacketIdSubL = MQTTClient_g.subscribe(TOPIC_STAT, 2);
	DEBUGLOG("Subscribing at QoS 2, packetId: %d\r\n", PacketIdSubL);

	PacketIdSubL = MQTTClient_g.subscribe(TOPIC_RELAY_IN, 2);
	DEBUGLOG("Subscribing at QoS 2, packetId: %d\r\n", PacketIdSubL);

	PacketIdSubL = MQTTClient_g.subscribe(TOPIC_UPDATE, 2);
	DEBUGLOG("Subscribing at QoS 2, packetId: %d\r\n", PacketIdSubL);
}

/**
 * @brief On MQTT Disconnect.
 * 
 * @param reason Reason for disconnect.
 */
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Disconnected from MQTT.\r\n");

	if (WiFi.isConnected()) {
		//mqttReconnectTimer.once(2, connectToMqtt);
	}
}

/**
 * @brief On MQTT Subscribe.
 * 
 * @param packetId Packet ID.
 * @param qos Quality od service.
 */
void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Subscribe acknowledged.\r\n");
	DEBUGLOG("  packetId: %d\r\n", packetId);
	DEBUGLOG("  qos: %d\r\n", qos);
}

/**
 * @brief On MQTT Unsubscribe.
 * 
 * @param packetId Packet ID.
 */
void onMqttUnsubscribe(uint16_t packetId) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Unsubscribe acknowledged.\r\n");
	DEBUGLOG("  packetId: %d\r\n", packetId);
}

/**
 * @brief On MQTT Message
 * 
 * @param topic Topic name.
 * @param payload Payload data.
 * @param properties Properties.
 * @param len Data length.
 * @param index Message index.
 * @param total Total message indexes.
 */
void onMqttMessage(
	char* topic,
	char* payload,
	AsyncMqttClientMessageProperties properties,
	size_t len,
	size_t index,
	size_t total) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	String tp = String(topic);

	// Update request.
	if (tp == TOPIC_UPDATE)
	{
		if ((uint8_t)payload[0] != (uint8_t)ESP_FW_VERSION)
		{
			DEBUGLOG("Time to update.\r\n");
			//check_update_ESP();
		}
		else
		{
			DEBUGLOG("No need to update.\r\n");
		}
	}

	// Serial out.
	if (tp == TOPIC_SER_OUT)
	{
		for (size_t idx = 0; idx < len; idx++)
		{
			Serial.print(payload[idx]);
		}
	}

	// Power control.
	if (tp == TOPIC_RELAY_IN)
	{
		if (payload[0] == '0')
		{
			power_off();
		}
		if (payload[0] == '1')
		{
			power_on();
		}
	}

	DEBUGLOG("Publish received.\r\n");
	DEBUGLOG("  topic: %s\r\n", topic);
	DEBUGLOG("  qos: %d\r\n", properties.qos);
	DEBUGLOG("  dup: %d\r\n", properties.dup);
	DEBUGLOG("  retain: %d\r\n", properties.retain);
	DEBUGLOG("  len: %d\r\n", len);
	DEBUGLOG("  index: %d\r\n", index);
	DEBUGLOG("  total: %d\r\n", total);
}

/**
 * @brief On MQTT Publish.
 * 
 * @param packetId Packet ID.
 */
void onMqttPublish(uint16_t packetId) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Publish acknowledged.\r\n");
	DEBUGLOG("  packetId: %d\r\n", packetId);
}

/**
 * @brief MQTT Begin.
 * 
 */
void mqtt_begin() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	MQTTClient_g.onConnect(onMqttConnect);
	MQTTClient_g.onDisconnect(onMqttDisconnect);
	MQTTClient_g.onSubscribe(onMqttSubscribe);
	MQTTClient_g.onUnsubscribe(onMqttUnsubscribe);
	MQTTClient_g.onMessage(onMqttMessage);
	MQTTClient_g.onPublish(onMqttPublish);
	MQTTClient_g.setServer(MqttConfiguration.Domain.c_str(), MqttConfiguration.Port);

	MQTTConnTimer_g.setExpirationTime(5000);
}

/**
 * @brief MQTT Reconnect.
 * 
 */
void mqtt_reconnect()
{
	// Check does the timer has expired.
	MQTTConnTimer_g.update();

	// If yes clear it and try to connect.
	if (MQTTConnTimer_g.expired())
	{
		MQTTConnTimer_g.clear();

		if (MqttConfiguration.Auth)
		{
			MQTTClient_g.setCredentials(
				MqttConfiguration.Username.c_str(),
				MqttConfiguration.Password.c_str());
		}

		MQTTClient_g.setClientId(NetworkConfiguration.Hostname.c_str());
		MQTTClient_g.connect();
	}
}

/**
 * @brief Read incoming data from the serial buffer.
 * 
 */
void read_device_serial()
{
	static String JSONMsgL = "";
	static String MessageL = "";
	static unsigned long long TSL = 0;

	if (COM_PORT.available() < 1)
	{
		return;
	}

	while (COM_PORT.available() > 0)
	{
		// Add new char.
		MessageL += (char)COM_PORT.read();
		// Wait a while for a a new char.
		delay(1);
		//ESP.wdtFeed();
		yield();
	}

	// If command if not empty parse it.
	if (MessageL != "")
	{
		// Get epoch time.
		TSL = (unsigned long long)NTPClient_g.getEpochTime();

		// Add miliseconds
		TSL *= 1000ULL;

		// Add the miliseconds part.
		TSL += (unsigned long long)(millis() % 999ULL);

		// Print it to the buffer.
		sprintf(TimestampBuff_g, "%llu", TSL);

		// Form the JSON message.
		JSONMsgL += "{\"ts\":";
		JSONMsgL += String(TimestampBuff_g);
		JSONMsgL += ", \"msg\":\"";
		JSONMsgL += MessageL;
		JSONMsgL += "\"}";

		// Publish message.
		MQTTClient_g.publish(TOPIC_SER_IN, 2, true, JSONMsgL.c_str());

		// Clear the command data buffer.
		JSONMsgL = "";
		MessageL = "";
	}
}

#pragma endregion

#pragma endregion

void setup()
{
	// Setup debug port module.
	setup_debug_port();

	//
	COM_PORT.begin(DeviceConfiguration.PortBaudrate);

	// Setup the relay.
	pinMode(PIN_RELAY, OUTPUT);

#ifdef ENABLE_RESCUE_BTN
	config_rescue_procedure();
#endif // ENABLE_RESCUE_BTN

	// Turn ON the self power latch.
	power_off();

#ifdef ENABLE_STATUS_LED
	// Setup the RGB led.
	StatusLed.init(PIN_RGB_LED);
#endif // ENABLE_STATUS_LED

	// Show flash state.
	show_device_properties();

	// Start the file system.
	configure_file_system();

	// Try to load configuration from file system. Load defaults if any error.
	if (!load_network_configuration(&SPIFFS, CONFIG_NET))
	{
		save_network_configuration(&SPIFFS, CONFIG_NET);
	}

	// Load HTTP authorization. Load defaults if any error.
	if (!load_device_config(&SPIFFS, CONFIG_DEVICE))
	{
		save_device_config(&SPIFFS, CONFIG_DEVICE);
	}

	// Load MQTT authorization. Load defaults if any error.
	if (!load_mqtt_configuration(&SPIFFS, CONFIG_MQTT))
	{
		save_mqtt_configuration(&SPIFFS, CONFIG_MQTT);
	}

	// If no SSID and Password wer set the go to AP mode.
	if (NetworkConfiguration.SSID != "" && NetworkConfiguration.Password != "")
	{
		configure_to_sta();
		NTPClient_g.update();
		DeviceStatusTimer_g.setExpirationTime(MQTT_HEARTBEAT_TIME);
#ifdef ENABLE_STATUS_LED
		StatusLed.setAnumation(AnimationType::Green);
#endif // ENABLE_STATUS_LED
	}
	else
	{
		configure_to_ap();
#ifdef ENABLE_STATUS_LED
		StatusLed.setAnumation(AnimationType::Blue);
#endif // ENABLE_STATUS_LED
	}

	DeviceStateTimer_g.setExpirationTime(1000);
	configure_web_server();

#ifdef ENABLE_IR_INTERFACE
	IRReceiver_g.enableIRIn();  // Start the receiver
#endif // ENABLE_IR_INTERFACE

#ifdef ENABLE_ARDUINO_OTA
	configure_arduino_ota();
#endif // ENABLE_ARDUINO_OTA

	if ((WiFi.getMode() == WIFI_STA) && WiFi.isConnected())
	{
		mqtt_begin();
	}
}

void loop()
{
#ifdef ENABLE_RESQUE_BTN
	update_rescue_procedure();
#endif // ENABLE_RESQUE_BTN

	// 
	AppWEBServer_g.update();
	DeviceStateTimer_g.update();
	if (DeviceStateTimer_g.expired())
	{
		DeviceStateTimer_g.clear();

		AppWEBServer_g.sendDeviceStatus(dev_status_to_json());

		// Update animation.
		AppWEBServer_g.sendDeviceState(dev_state_to_json());

		// Below code will be removed after release.
		DeviceState.BumpersAndWheelDrops++;
		DeviceState.Wall = !DeviceState.Wall;
		DeviceState.CliffLeft = !DeviceState.CliffLeft;
		DeviceState.CliffFrontLeft = !DeviceState.CliffFrontLeft;
		DeviceState.CliffFrontRight = !DeviceState.CliffFrontRight;
		DeviceState.CliffRight = !DeviceState.CliffRight;

		if (DeviceState.BumpersAndWheelDrops > 15)
		{
			DeviceState.BumpersAndWheelDrops = 0;
		}

		//AppWEBServer_g.displayLog(dev_state_to_json());
	}

	// If everything is OK with the transport layer.
	if ((WiFi.getMode() == WIFI_STA) && WiFi.isConnected())
	{
		// Update date and time.
		NTPClient_g.update();

		// Reconnect MQTT if necessary.
		if (!MQTTClient_g.connected())
		{
			mqtt_reconnect();
		}
		else
		{
			read_device_serial();
		}

		// If heartbeat expired then run trough.
		DeviceStatusTimer_g.update();
		if (DeviceStatusTimer_g.expired())
		{
			DeviceStatusTimer_g.clear();
			//DEBUGLOG("Time: %s\r\n", NTPClient_g.getFormattedTime().c_str());

			DeviceStatus.Timestamp = NTPClient_g.getEpochTime(); // -DeviceConfiguration.NTPTimezone;
#ifdef ESP32
			DeviceStatus.Voltage = battery_voltage(PIN_BATT);
#elif defined(ESP8266)
			DeviceStatus.Voltage = 0.0F;
#endif
			DeviceStatus.RSSI = WiFi.RSSI();
			DeviceStatus.SSID = NetworkConfiguration.SSID;
			DeviceStatus.Flags = 0; // TODO: Flags
			DeviceStatus.FreeHeap = ESP.getFreeHeap();

			// part of the flags. - MQTTClient_g.connected();

			if (MQTTClient_g.connected())
			{
				MQTTClient_g.publish(TOPIC_STAT, 0, true, dev_status_to_json().c_str());
			}
		}
	}

#ifdef ENABLE_ARDUINO_OTA
	ArduinoOTA.handle();
#endif // ENABLE_ARDUINO_OTA

#ifdef ENABLE_IR_INTERFACE
	if (IRReceiver_g.decode(&IRResults_g))
	{
		AppWEBServer_g.displayIRCommand(IRResults_g.command);
		IRReceiver_g.resume();  // Receive the next value
		if (IRResults_g.command != 0)
		{
#ifdef ENABLE_DEVICE_CONTROL
			if (IRResults_g.command == DeviceConfiguration.ActivationCode)
			{
				if (CommandModule.getState() == StateType::Start)
				{
					CommandModule.setState(StateType::Stop);
				}
				else if (CommandModule.getState() == StateType::Stop)
				{
					CommandModule.setState(StateType::Restart);
				}
				else if (CommandModule.getState() == StateType::Restart)
				{
					CommandModule.setState(StateType::Start);
				}
			}
			if (MQTTClient_g.connected())
			{
				MQTTClient_g.publish(TOPIC_IR, 2, true, String(IRResults_g.command).c_str());
			}
#endif // ENABLE_DEVICE_CONTROL
		}
	}
#endif // ENABLE_IR_INTERFACE

#ifdef ENABLE_STATUS_LED
	StatusLed.update();
#endif // ENABLE_STATUS_LED
}
