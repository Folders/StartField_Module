#include "Peripherals/M_Flash.h"
#include <Arduino.h>
#include <common.h>

M_Flash::M_Flash(char color)
{
    // Save local variable
    _color = color;
}


void M_Flash::begin(uint8_t buttonPin)
{
	// Config of button
    _pin = buttonPin;
	pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);

    #ifdef DEBUG
	Serial.print("FlashLamp ");
	Serial.print(_color);
	Serial.println(": Started");
    #endif
}


void M_Flash::setFlash(char value)
{
    if (value == '1')
        digitalWrite(_pin, HIGH);
    else
        digitalWrite(_pin, LOW);
        
    #ifdef DEBUG
	Serial.print("FlashLamp ");
	Serial.print(_color);
	Serial.print(": State change to ");
	Serial.println(value);
    #endif
}   