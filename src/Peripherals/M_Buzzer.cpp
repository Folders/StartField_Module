#include "Peripherals/M_Buzzer.h"
#include <Arduino.h>
#include <common.h>

void M_Buzzer::begin(uint8_t buttonPin)
{
	// Config of button
    _pin = buttonPin;
	pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);

    #ifdef DEBUG
	Serial.println("Buzzer: Started");
    #endif
}


void M_Buzzer::buzz(const char* time)
{
	// Start buzzer
	digitalWrite(_pin, 1);

    // Set ticker to wait the buzzer time
    _ticker.attach(CharToFloat(time), std::bind(&M_Buzzer::_endBuzz, this));

#ifdef DEBUG
	Serial.print("Buzzer: Run during ");
	Serial.print(time);
	Serial.println(" s");
#endif
}

void M_Buzzer::_endBuzz()
{
	// Stop buzzer
	_ticker.detach();
	digitalWrite(_pin, 0);

#ifdef DEBUG
	Serial.println("Buzzer: Stopped");
#endif
}
