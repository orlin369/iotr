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
	// Get time.
	m_now = millis();

	// Calculate passed since last time.
	m_passedTime = m_now - m_lastTime;

	// Chack passed time, is it grater then allowd.
	if (m_passedTime >= m_expirationTime)
	{
		if (m_expired == false)
		{
			m_lastTime = m_now;
			m_expired = true;
		}

		if (m_callbackExpiration != nullptr)
		{
			m_callbackExpiration(m_now);
		}
	}
}

void FxTimer::updateLastTime()
{
	m_lastTime = millis();
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
void FxTimer::setExpirationCb(void(*callback)(unsigned long now))
{
	m_callbackExpiration = callback;
}

void FxTimer::setExpirationTime(unsigned long time)
{
	m_expirationTime = time;
}

unsigned long FxTimer::getExpirationTime()
{
	return m_expirationTime;
}

bool FxTimer::expired()
{
	return m_expired;
}
