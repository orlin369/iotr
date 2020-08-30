// 
// 
// 

#include "RescueButton.h"

FxTimer RescueTimer_g;

void config_rescue_procedure() {
#ifdef SHOW_FUNC_NAMES
	DEBUGLOG("\r\n");
	DEBUGLOG(__PRETTY_FUNCTION__);
	DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES
	
	pinMode(PIN_DEVICE_RESCUE, INPUT_PULLUP);

	RescueTimer_g = FxTimer();
	RescueTimer_g.setExpirationTime(RESCUE_DEVICE_TIME);
}

void update_rescue_procedure()
{
	if (digitalRead(PIN_DEVICE_RESCUE) == LOW)
	{
		RescueTimer_g.update();
		if (RescueTimer_g.expired())
		{
			RescueTimer_g.clear();
			DEBUGLOG("-===RESCUE===-\r\n");
			// Clear device.
			set_default_device_config();
			save_device_config(&SPIFFS, CONFIG_DEVICE);

			// Clear network.
			set_default_network_configuration();
			save_network_configuration(&SPIFFS, CONFIG_NET);

			// TODO: Set the LED to yellow.

			// Reboot device.
			ESP.restart();
		}
	}
	else
	{
		RescueTimer_g.updateLastTime();
	}
}
