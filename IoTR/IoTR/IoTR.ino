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

#pragma region Headers

/* Application configuration. */
#include "ApplicationConfiguration.h"

// SDK
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <WiFiClient.h>
#include <FS.h>

#include <AsyncMqttClient.h>
#include <NTPClient.h>

/* Debug serial port. */
#include "DebugPort.h"

/* Device configuration. */
#include "NetworkConfiguration.h"

/* Smart scale WEB server. */
#include "WEBServer.h"

#include "FxTimer.h"

#include "DeviceState.h"

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

#pragma endregion

#pragma region Variables

WiFiUDP NTP_UDP_g;

NTPClient NTPClient_g(NTP_UDP_g);

FxTimer WiFiConnTimer_g = FxTimer();

FxTimer MQTTConnTimer_g = FxTimer();

FxTimer HeartbeatTimer_g = FxTimer();

FxTimer DeviceStateTimer_g = FxTimer();

AsyncMqttClient MQTTClient_g;

#ifdef ENABLE_IR_INTERFACE

/* @brief IR receiver. */
IRrecv IRReciver_g(PIN_IR_RECV);

/* @brief IR receiver result. */
decode_results IRResults_g;

#endif // ENABLE_IR_INTERFACE

char TimestampBuff_g[18];

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
	DEBUGLOG("MAC address: %s\r\n", WiFi.macAddress().c_str());
	DEBUGLOG("\r\n");
}

#pragma region SOFTWARE ON OFF

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
	WiFi.hostname(NetworkConfiguration.Hostname.c_str());

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
	WiFi.hostname(NetworkConfiguration.Hostname.c_str());

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
	LocalWEBServer.setCbStopDevice(power_off);
	LocalWEBServer.setCbStartDevice(power_on);
	LocalWEBServer.setCbReboot(reboot);
}

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
			MQTTClient_g.setCredentials(
				MqttConfiguration.Username.c_str(),
				MqttConfiguration.Password.c_str());
		}

		MQTTClient_g.setClientId(NetworkConfiguration.Hostname.c_str());
		MQTTClient_g.connect();
	}
}

/** @brief Read incoming data from the serial buffer.
 *  @return Void.
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
		delay(5);
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
		TSL += (unsigned long long)(millis() % 999UL);

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

#ifdef ENABLE_ROOMBA

void wakeUp(void) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	int ddPin = D1;
	pinMode(ddPin, OUTPUT);
	digitalWrite(ddPin, HIGH);
	delay(100);
	digitalWrite(ddPin, LOW);
	delay(500);
	digitalWrite(ddPin, HIGH);
	delay(2000);
}

void startSafe() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	COM_PORT.write(128);  //Start
	COM_PORT.write(131);  //Safe mode
	delay(1000);
}

void drive(int velocity, int radius) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	clamp(velocity, -500, 500); //def max and min velocity in mm/s
	clamp(radius, -2000, 2000); //def max and min radius in mm

	COM_PORT.write(137);
	COM_PORT.write(velocity >> 8);
	COM_PORT.write(velocity);
	COM_PORT.write(radius >> 8);
	COM_PORT.write(radius);
}

void turnCW(unsigned short velocity, unsigned short degrees) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	drive(velocity, -1);
	clamp(velocity, 0, 500);
	delay(6600);
	drive(0, 0);
}
#endif // ENABLE_ROOMBA

void setup()
{
	// Setup debug port module.
	setup_debug_port();

	//
	COM_PORT.begin(DeviceConfiguration.PortBaudrate);

	// Setup the relay.
	pinMode(PIN_RELAY, OUTPUT);

#ifdef ENABLE_ROOMBA
	wakeUp();
	startSafe();
	turnCW(40, 180);
#endif // ENABLE_ROOMBA

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
		HeartbeatTimer_g.setExpirationTime(MQTT_HEARTBEAT_TIME);
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
#ifdef ENABLE_RESQUE_BTN
	update_recue_procedure();
#endif // ENABLE_RESQUE_BTN

	// 
	LocalWEBServer.update();
	DeviceStateTimer_g.update();
	if (DeviceStateTimer_g.expired())
	{
		DeviceStateTimer_g.clear();

		LocalWEBServer.sendDeviceStatus(dev_status_to_json());

		LocalWEBServer.sendDeviceState(dev_state_to_json());

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
		HeartbeatTimer_g.update();
		if (HeartbeatTimer_g.expired())
		{
			HeartbeatTimer_g.clear();
			//DEBUGLOG("Time: %s\r\n", NTPClient_g.getFormattedTime().c_str());

			DeviceStatus.Timestamp = NTPClient_g.getEpochTime(); // -DeviceConfiguration.NTPTimezone;
			DeviceStatus.Voltage = (int)map(analogRead(A0), 0, 1023, 0, 3.3);
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
	if (IRReciver_g.decode(&IRResults_g))
	{
		LocalWEBServer.displayIRCommand(IRResults_g.command);
		IRReciver_g.resume();  // Receive the next value
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
