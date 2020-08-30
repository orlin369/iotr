// RescueButton.h

#ifndef _RESCUEBUTTON_h
#define _RESCUEBUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ApplicationConfiguration.h"
#include "DebugPort.h"
#include "FxTimer.h"
#include "NetworkConfiguration.h"
#include "DeviceConfiguration.h"

#ifndef RESCUE_DEVICE_TIME
#define RESCUE_DEVICE_TIME 5000
#endif // !RESCUE_DEVICE_TIME

#ifndef PIN_DEVICE_RESCUE
#define PIN_DEVICE_RESCUE 5
#endif // !PIN_DEVICE_RESCUE

void config_rescue_procedure();

void update_rescue_procedure();

#endif

