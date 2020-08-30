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

// StatusLed.h

#ifndef _STATUSLED_h
#define _STATUSLED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Headers

#include "ApplicationConfiguration.h"

#include "DebugPort.h"

#include <Adafruit_NeoPixel.h>

#include "FxTimer.h"

#pragma endregion

/** @brief Frame indexes byte. */
enum AnimationType : int
{
	Red = 0, ///< Red fade animation.
	Green, ///< Green fade animation.
	Blue, ///< Blue fade animation.
};

class StatusLedClass
{
 protected:
	 Adafruit_NeoPixel m_pixels;
	 FxTimer m_animationTimer;
	 int m_animationType;

 public:
	void init(uint8_t pin);
	void update();
	void setAnumation(int animationType);
};

extern StatusLedClass StatusLed;

#endif

