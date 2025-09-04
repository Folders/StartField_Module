#pragma once
#include <Arduino.h>
#include <Ticker.h>
#include <PA_LCD.h>

/// @brief Classe représentant le module de type BASE
class M_Button {

public:

    M_Button(char color);

    /// @brief Initialisation du module
    void begin(uint8_t buttonPin);

	void readButton();

    void setLed(char state);

    bool value();
    bool up();
    bool down();

private:
                 
    PA_LCD _lcd = PA_LCD();
    
    char _color;
    uint8_t _pin;
    bool _popup = false;

    uint8_t _buttonPin;
	bool _up;
	bool _down;

    uint8_t _state;
    uint8_t _backup;

    // Timmer
    Ticker _ticker;
    void T_500ms();
    bool _blinkState;
};
