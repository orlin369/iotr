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

// DeviceStatus.h

#ifndef _DEVICESTATUS_h
#define _DEVICESTATUS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ApplicationConfiguration.h"

#include <ArduinoJson.h>

/*
{
	"ts": 1584114705,
	"flags": 7,
	"target_voltage": 5.0,
	"ssid": "name",
	"rssi": -97,

}
*/

typedef struct {
	unsigned long Timestamp = 0; ///< Timestamp.
	unsigned int Flags = 0; ///< Status flags.
	float Voltage = 0; ///< Device voltage.
	String SSID = ""; ///< Device name.
	int RSSI = 0; ///< Device name.
} DeviceStatus_t;

/* @brief Singelton device stater instance. */
extern DeviceStatus_t DeviceStatus;

String dev_status_to_json();

#endif

