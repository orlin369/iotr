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

#pragma region Headers

/* Application configuration. */
#include "ApplicationConfiguration.h"

// SDK
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif // ESP8266
#ifdef ESP32
#include <WiFi.h>
#endif // ESP32
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <FS.h>

#include <AsyncMqttClient.h>

/* Debug serial port. */
#include "DebugPort.h"

/* Device configuration. */
#include "NetworkConfiguration.h"

/* Smart scale WEB server. */
#include "WEBServer.h"

#include "CommandModule.h"

#include "FxTimer.h"

#ifdef ENABLE_ARDUINO_OTA
#include <ArduinoOTA.h>
#endif // ENABLE_ARDUINO_OTA

#ifdef ENABLE_HTTP_OTA
// TODO: Make call from the device monitoring service.
#include "UpdatesManager.h"
#endif // ENABLE_HTTP_OTA

#ifdef ENABLE_IR_INTERFACE
#ifdef ESP8266
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#endif // ESP8266
#ifdef ESP32
#include <IRremote.h>
#endif // ESP32
#endif // ENABLE_IR_INTERFACE

#ifdef ENABLE_STATUS_LED
#include "StatusLed.h"
#endif // ENABLE_STATUS_LED

#pragma endregion

#pragma region Variables

FxTimer WiFiConnTimer_g = FxTimer();

FxTimer MQTTConnTimer_g;

AsyncMqttClient MQTTClient_g;

#ifdef ENABLE_IR_INTERFACE

/* @brief IR reciver. */
IRrecv IRReciver_g(RECV_PIN);

/* @brief IR reciever result. */
decode_results IRResults_g;

#endif // ENABLE_IR_INTERFACE

#pragma endregion

#pragma region Prototypes

/** @brief Configure the file system.
 *  @param fileSystem FS, File system object.
 *  @return Void.
 */
void configure_file_system();

/** @brief Configure WiFi module to access point.
 *  @return Void.
 */
void configure_to_ap();

/** @brief Handler that execute when client is connected.
 *  @param const WiFiEventSoftAPModeStationConnected& evt, Callback handler
 *  @return Void.
 */
void handler_ap_mode_station_connected(const WiFiEventSoftAPModeStationConnected& evt);

/** @brief Handler that execute when client is disconnected.
 *  @param const WiFiEventSoftAPModeStationDisconnected& evt, Callback handler
 *  @return Void.
 */
void handler_ap_mode_station_disconnected(const WiFiEventSoftAPModeStationDisconnected& evt);

/** @brief Configure WiFi module to station.
 *  @return Void.
 */
void configure_to_sta();

/** @brief Printout in the debug console flash state.
 *  @return Void.
 */
void show_device_properties();

#ifdef ENABLE_ARDUINO_OTA
void configure_arduino_ota();
#endif // ENABLE_ARDUINO_OTA

#pragma endregion

#pragma region Functions

/** @brief Printout in the debug console flash state.
 *  @return Void.
 */
void show_device_properties() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Flash chip size: %u\r\n", ESP.getFlashChipRealSize());
	DEBUGLOG("Sketch size: %u\r\n", ESP.getSketchSize());
	DEBUGLOG("Free flash space: %u\r\n", ESP.getFreeSketchSpace());
	DEBUGLOG("Free heap: %d\r\n", ESP.getFreeHeap());
	DEBUGLOG("Firmware version: %d\r\n", ESP_FW_VERSION);
	DEBUGLOG("SDK version: %s\r\n", ESP.getSdkVersion());
	uint8 c_MACL[WL_MAC_ADDR_LENGTH];
	WiFi.macAddress(c_MACL);
	String MACL = mac2str(c_MACL);
	DEBUGLOG("MAC address: %s\r\n", MACL.c_str());
	DEBUGLOG("\r\n");
}

#pragma region SOFTWARE ON OFF

void power_on() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#ifdef EANBLE_SOFTWARE_ONOFF
	pinMode(PIN_PON, OUTPUT);
	digitalWrite(PIN_PON, LOW);
#endif // EANBLE_SOFTWARE_ONOFF
}

void power_off() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#ifdef EANBLE_SOFTWARE_ONOFF
	pinMode(PIN_PON, OUTPUT);
	digitalWrite(PIN_PON, LOW);
#endif // EANBLE_SOFTWARE_ONOFF
}

#pragma endregion

#pragma region File System

/** @brief Configure the file system.
 *  @param fileSystem FS, File system object.
 *  @return Void.
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
			ESP.wdtFeed();
		}
	}

#ifdef DEBUGLOG
	// List files
	Dir dir = SPIFFS.openDir("/");
	while (dir.next()) {
		String fileName = dir.fileName();
		size_t fileSize = dir.fileSize();
		DEBUGLOG("File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
	}
	DEBUGLOG("\r\n");
#endif // DEBUGLOG
}

#pragma endregion

#pragma region AP mode

/** @brief Configure WiFi module to access point.
 *  @return Void.
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
	WiFi.hostname(DeviceConfiguration.DeviceName.c_str());

	// Set the mode.
	WiFi.mode(WIFI_AP);

	// NOTE: See description, this can be used as unique identification of the device.
	AccessPointConfiguration.SSID = String(DEVICE_BRAND) + String(" - ") + (String)ESP.getChipId();

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

#pragma endregion

#pragma region STA mode

/** @brief Configure WiFi module to station.
 *  @return Void.
 */
void configure_to_sta() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// Set the host name.
	WiFi.hostname(DeviceConfiguration.DeviceName.c_str());

	//disconnect required here
	//improves reconnect reliability
	WiFi.disconnect();

	// Encourage clean recovery after disconnect species5618, 08-March-2018
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

	WiFiConnTimer_g.setExpirationTime(TIMEOUT_TO_CONNECT * 1000);
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
		ESP.wdtFeed();
	}

	DEBUGLOG("Connected:  %s\r\n", NetworkConfiguration.SSID.c_str());
	DEBUGLOG("IP Address: %s\r\n", WiFi.localIP().toString().c_str());
	DEBUGLOG("Gateway:    %s\r\n", WiFi.gatewayIP().toString().c_str());
	DEBUGLOG("DNS:        %s\r\n", WiFi.dnsIP().toString().c_str());
	DEBUGLOG("\r\n");
}

#pragma endregion

#pragma region Local WEB Server

void configure_web_server() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	LocalWEBServer.begin(&SPIFFS);
	LocalWEBServer.setCbStopDevice(stop_device);
	LocalWEBServer.setCbStartDevice(start_device);
}

#pragma endregion

#pragma region Command Module

void configure_command_module() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#ifdef ENABLE_DEVICE_CONTROL
	CommandModule.init(PIN_LED2, PIN_LED3);
	CommandModule.restart();
#endif // ENABLE_DEVICE_CONTROL
}

void stop_device() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#ifdef ENABLE_DEVICE_CONTROL
	CommandModule.stop();
#endif // ENABLE_DEVICE_CONTROL
}

void start_device() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#ifdef ENABLE_DEVICE_CONTROL
	CommandModule.start();
#endif // ENABLE_DEVICE_CONTROL
}

#pragma endregion

#pragma region Arduino OTA Updates

#ifdef ENABLE_ARDUINO_OTA

void configure_arduino_ota() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	// Port defaults to 8266
	// ArduinoOTA.setPort(8266);

	// Hostname defaults to esp8266-[ChipID]
	// ArduinoOTA.setHostname("myesp8266");

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

#pragma region MQTT Service

void onMqttConnect(bool sessionPresent) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Connected to MQTT.\r\n");
	DEBUGLOG("Session present: %d\r\n", sessionPresent);

	uint16_t packetIdSub = MQTTClient_g.subscribe("test/lol", 2);
	DEBUGLOG("Subscribing at QoS 2, packetId: %d\r\n", packetIdSub);

	MQTTClient_g.publish("test/lol", 0, true, "test 1");
	DEBUGLOG("Publishing at QoS 0\r\n");

	uint16_t packetIdPub1 = MQTTClient_g.publish("test/lol", 1, true, "test 2");
	DEBUGLOG("Publishing at QoS 1, packetId: %d\r\n", packetIdPub1);

	uint16_t packetIdPub2 = MQTTClient_g.publish("test/lol", 2, true, "test 3");
	DEBUGLOG("Publishing at QoS 2, packetId: %d\r\n", packetIdPub2);
}

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

void onMqttUnsubscribe(uint16_t packetId) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Unsubscribe acknowledged.\r\n");
	DEBUGLOG("  packetId: %d\r\n", packetId);
}

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

	DEBUGLOG("Publish received.\r\n");
	DEBUGLOG("  topic: %d\r\n", topic);
	DEBUGLOG("  qos: %d\r\n", properties.qos);
	DEBUGLOG("  dup: %d\r\n", properties.dup);
	DEBUGLOG("  retain: %d\r\n", properties.retain);
	DEBUGLOG("  len: %d\r\n", len);
	DEBUGLOG("  index: %d\r\n", index);
	DEBUGLOG("  total: %d\r\n", total);
}

void onMqttPublish(uint16_t packetId) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	DEBUGLOG("Publish acknowledged.\r\n");
	DEBUGLOG("  packetId: %d\r\n", packetId);
}

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

	MQTTConnTimer_g = FxTimer();
	MQTTConnTimer_g.setExpirationTime(5000);
}

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
			MQTTClient_g.setCredentials(MqttConfiguration.Username.c_str(),
				MqttConfiguration.Password.c_str());
			MQTTClient_g.setClientId(DeviceConfiguration.DeviceName.c_str());
		}
		else
		{
			MQTTClient_g.setClientId(DeviceConfiguration.DeviceName.c_str());
		}

		MQTTClient_g.connect();
	}
}

#pragma endregion

#pragma endregion

void setup()
{
	// Setup debug port module.
	setup_debug_port();

	// Turn ON the self power latch.
	power_on();

	// Configure command module.
	configure_command_module();

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

	configure_web_server();

#ifdef ENABLE_IR_INTERFACE
	IRReciver_g.enableIRIn();  // Start the receiver
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
	// 
	LocalWEBServer.update();

	// 
	if ((WiFi.getMode() == WIFI_STA) && WiFi.isConnected())
	{
		if (!MQTTClient_g.connected())
		{
			mqtt_reconnect();
		}
	}

#ifdef ENABLE_ARDUINO_OTA
	ArduinoOTA.handle();
#endif // ENABLE_ARDUINO_OTA

#ifdef ENABLE_IR_INTERFACE
	if (IRReciver_g.decode(&IRResults_g))
	{
		LocalWEBServer.displayIRCommand(IRResults_g.command);
		IRReciver_g.resume();  // Receive the next value
	}
#endif // ENABLE_IR_INTERFACE

#ifdef ENABLE_STATUS_LED
	StatusLed.update();
#endif // ENABLE_STATUS_LED
}
