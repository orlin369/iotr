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

// CommandModule.h

#ifndef _COMMANDMODULE_h
#define _COMMANDMODULE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ApplicationConfiguration.h"

#include "DebugPort.h"

class CommandModuleClass
{
 protected:
	 uint8_t m_pinStart;
	 uint8_t m_pinKill;

 public:
	void init(uint8_t pin_start, uint8_t pin_kill);
	void restart();
	void start();
	void stop();

};

extern CommandModuleClass CommandModule;

#endif

