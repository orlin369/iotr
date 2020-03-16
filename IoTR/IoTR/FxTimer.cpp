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

#include "FxTimer.h"

void FxTimer::update()
{
	m_now = millis();

	unsigned long PassedTimeL = m_now - m_last_time;

	if (PassedTimeL >= m_expiration_time)
	{
		if (m_expired == false)
		{
			m_last_time = m_now;
			m_expired = true;
		}

		if (callbackExpiration != nullptr)
		{
			callbackExpiration(millis());
		}
	}
}

void FxTimer::updateLastTime()
{
	m_last_time = millis();
}

void FxTimer::clear()
{
	if (m_expired == true)
	{
		m_expired = false;
	}
}

/** @brief Set callback for expiration time.
 *  @param Callback function.
 *  @return Void.
 */
void FxTimer::setExpirationCb(void(*callback)(unsigned long day))
{
	callbackExpiration = callback;
}

void FxTimer::setExpirationTime(unsigned long time)
{
	m_expiration_time = time;
}

unsigned long FxTimer::getExpirationTime()
{
	return m_expiration_time;
}


bool FxTimer::expired()
{
	return m_expired;
}
