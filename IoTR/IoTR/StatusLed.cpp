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

#include "StatusLed.h"

void StatusLedClass::init(uint8_t pin) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_animationTimer = FxTimer();
	m_animationTimer.setExpirationTime(50);

	m_pixels = Adafruit_NeoPixel(1, pin, NEO_RGB + NEO_KHZ800);
	m_pixels.begin();

}

void StatusLedClass::update()
{
	static uint8_t FadeValueL = 0;
	static bool FadeDirectionL = false;

	m_animationTimer.update();
	if (m_animationTimer.expired())
	{
		m_animationTimer.clear();

		if (FadeDirectionL == false)
		{
			FadeValueL++;
			if (FadeValueL > 33)
			{
				FadeDirectionL = true;
			}
		}
		else
		{
			FadeValueL--;
			if (FadeValueL < 1)
			{
				FadeDirectionL = false;
			}
		}

		if (m_animationType == AnimationType::Red)
		{
			m_pixels.setPixelColor(0, m_pixels.Color(FadeValueL, 0, 0));
		}
		else if (m_animationType == AnimationType::Green)
		{
			m_pixels.setPixelColor(0, m_pixels.Color(0, FadeValueL, 0));
		}
		else if (m_animationType == AnimationType::Blue)
		{
			m_pixels.setPixelColor(0, m_pixels.Color(0, 0, FadeValueL));
		}

		m_pixels.show();
	}
}

void StatusLedClass::setAnumation(int animationType) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_animationType = animationType;
}

StatusLedClass StatusLed;

