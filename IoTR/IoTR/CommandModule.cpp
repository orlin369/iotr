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

#include "CommandModule.h"

void CommandModuleClass::init(uint8_t pin_start, uint8_t pin_kill) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_pinStart = pin_start;
	m_pinKill = pin_kill;

	pinMode(m_pinStart, OUTPUT);
	pinMode(m_pinKill, OUTPUT);

	stop();
}

void CommandModuleClass::restart() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	digitalWrite(m_pinStart, HIGH);
	digitalWrite(m_pinKill, LOW);
}

void CommandModuleClass::start() {
/*
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES
*/
	digitalWrite(m_pinStart, HIGH);
	digitalWrite(m_pinKill, HIGH);
}

void CommandModuleClass::stop() {
/*
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES
*/
	digitalWrite(m_pinStart, LOW);
	digitalWrite(m_pinKill, LOW);
}

void CommandModuleClass::setState(int state) {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

	m_state = state;
	DEBUGLOG("State: %d\r\n", m_state);
}

int CommandModuleClass::getState()
{
	return m_state;
}

void CommandModuleClass::update()
{
	if (!m_enabled)
	{
		this->stop();
		return;
	}

	if (m_state == StateType::Start)
	{
		this->start();
	}
	else if (m_state == StateType::Stop)
	{
		this->stop();
	}
	else if (m_state == StateType::Restart)
	{
		this->restart();
	}
}

void CommandModuleClass::enable(bool enabled)
{
	this->m_enabled = enabled;
}

CommandModuleClass CommandModule;

