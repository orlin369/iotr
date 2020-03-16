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

// DeviceState.h

#ifndef _DEVICESTATE_h
#define _DEVICESTATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ApplicationConfiguration.h"

#include <ArduinoJson.h>

typedef struct {
	bool Wall = false; ///< Wall sensor.
	bool CliffLeft = false; ///< Password - WWW password. Set default value.
	bool CliffFrontLeft = false; ///< Device name.
	bool CliffFrontRight = false; ///< Device name.
	bool CliffRight = false; ///< Device name.
	uint8_t BumpersAndWheelDrops = false; ///< Device name.
} DeviceState_t;

/* @brief Singelton device state. */
extern DeviceState_t DeviceState;

String dev_state_to_json();

#endif

