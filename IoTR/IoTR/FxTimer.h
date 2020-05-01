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

// FxTimer.h

#ifndef _FXTIMER_h
#define _FXTIMER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class FxTimer
{
 protected:

	 /** @brief Expired flag. */
	 bool m_expired = false;

	 /** @brief Expiration time. */
	 unsigned long m_expirationTime = 0;
	 
	 /** @brief Last time. */
	 unsigned long m_lastTime = 0;
	 
	 /** @brief Current time. */
	 unsigned long m_now = 0;

	 /** @brief Passted time. */
	 unsigned long m_passedTime = 0;
	 
	 /** @brief Callback when expire. */
	 void(*m_callbackExpiration)(unsigned long now);

 public:

	 void update();

	 void updateLastTime();

	 void clear();

	 void setExpirationCb(void(*callback)(unsigned long now));

	 void setExpirationTime(unsigned long);

	 unsigned long getExpirationTime();

	 bool expired();
};

#endif
