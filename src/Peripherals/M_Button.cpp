#include "Peripherals/M_Button.h"
#include "core/MyComm.h"
#include <Arduino.h>
#include <common.h>

M_Button::M_Button(char color)
{
    // Save local variable
    _color = color;
}


void M_Button::begin(uint8_t buttonPin)
{
	// Config of button
    _pin = buttonPin;
	pinMode(_pin,INPUT);
	digitalWrite(_pin,HIGH);

    // First read
	_state = digitalRead(_pin);
	_backup = _state;

    #ifdef DEBUG
	Serial.print("Button ");
	Serial.print(_color);
	Serial.println(": Started");
    #endif
}


void M_Button::readButton(void){


	_state = _lcd.read(_color);
    
	if(_backup != _state)
	{
		_backup = _state;

        char msg[] = "BTN;X;Y";   // 8 octets, indices 0..7 + '\0'
        msg[4] = _color;           // remplace X
        msg[6] = (_state == HIGH) ? '0' : '1';  // remplace Y

        comm.send(msg);

		if (_state == HIGH) {
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


void M_Button::setLed(char state){


    #ifdef DEBUG
	Serial.print("Change LED ");
	Serial.print(_color);
	Serial.print(" to ");
	Serial.println(state);
    #endif
    
		// Chagement de la couleur
	switch (state)
	{
	case '0': // Rouge
		_lcd.setLed(_color, 0);
    	_ticker.detach();
		break;
	case '1': // Jaune
		_lcd.setLed(_color, 1);
    	_ticker.detach();
		break;
	case 'B': // Vert
		_blinkState = true;
		_lcd.setLed(_color, 1);
		_ticker.attach(0.5, std::bind(&M_Button::T_500ms, this));
		break;
	}

    bool var = state == '1' ? 1 : 0;

	_lcd.setLed(_color, var);
	
}

void M_Button::T_500ms()
{
	// Toggle bit
	_blinkState = !_blinkState;
	_lcd.setLed(_color, _blinkState);
  
}
/*
|| Return value of switch
*/
bool M_Button::value(void){
	
    if (_state){
        return true;
    } else {
        return false;
    }
}

/*
|| Return flank UP
*/
bool M_Button::up(void){
    return _up;
}

/*
|| Return flank DOWN
*/
bool M_Button::down(void){
    return _down;
}