
#ifndef PA_Bouton_H
#define PA_Bouton_H

#include "Arduino.h"

class PA_Bouton{
  public:
    PA_Bouton(uint8_t buttonPin);
	void read();
	void readLCD(uint8_t value);
    bool value();
    bool up();
    bool down();
  private: 
    uint8_t pin;
    uint8_t state;
    uint8_t backup;
	bool _up;
	bool _down;
};

#endif

