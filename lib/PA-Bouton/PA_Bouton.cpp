//include the class definition
#include "PA_Bouton.h"
#include <PA_LCD.h>
#include <utility/Adafruit_MCP23017.h>

/*
|| <<constructor>>
|| @parameter buttonPin sets the pin that this switch is connected to
*/
PA_Bouton::PA_Bouton(uint8_t buttonPin){
	// Save pin
	this->pin=buttonPin;
		
	// Config of button
	pinMode(pin,INPUT);
	digitalWrite(pin,HIGH);
	
	state = digitalRead(pin);

	backup = state;
}

/*
|| Read value of button
*/
void PA_Bouton::read(void){


	state = digitalRead(pin);
	
	if(backup != state)
	{
		backup = state;
 
		if (state == HIGH) {
			_down = true;
		} else {   
			_up = true;
		}
	}
	else{
		_down = false;
		_up = false;
	}
}

/*
|| Read value of button
*/
void PA_Bouton::readLCD(uint8_t value){


	state = value;
	
	if(backup != state)
	{
		backup = state;
 
		if (state == HIGH) {
			_down = true;
		} else {   
			_up = true;
		}
	}
	else{
		_down = false;
		_up = false;
	}
}

/*
|| Return value of switch
*/
bool PA_Bouton::value(void){
	
    if (state){
        return true;
    } else {
        return false;
    }
}

/*
|| Return flank UP
*/
bool PA_Bouton::up(void){
    return _up;
}

/*
|| Return flank DOWN
*/
bool PA_Bouton::down(void){
    return _down;
}





