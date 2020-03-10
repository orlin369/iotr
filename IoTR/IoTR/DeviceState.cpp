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

// 
// 
// 

#include "DeviceState.h"

/* @brief Singelton Smart scale server instance. */
DeviceState_t DeviceState;

String dev_state_to_json(DeviceState_t* state) {
/*
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES
*/

	//flag_config = false;
	DynamicJsonDocument doc(512);

	doc["Wall"] = state->Wall;
	doc["CliffLeft"] = state->CliffLeft;
	doc["CliffFrontLeft"] = state->CliffFrontLeft;
	doc["CliffFrontRight"] = state->CliffFrontRight;
	doc["CliffRight"] = state->CliffRight;
	doc["BumpersAndWheelDrops"] = state->BumpersAndWheelDrops;

	String content = "";

	serializeJson(doc, content);
	//DEBUGLOG("%s\r\n", content.c_str());

	return content;
}