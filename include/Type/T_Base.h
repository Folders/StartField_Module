#ifndef T_BASE_H
#define T_BASE_H

#include <Arduino.h>
#include "Core/Communication.h"
#include "Peripherals/M_LCD.h"
#include "Peripherals/M_Button.h"
#include "Peripherals/M_Buzzer.h"
#include "Peripherals/M_Flash.h"



void SetBase(char Id, char Team, char Type, char Spawn);

void SetBaseTime(char Min[], char Sec[]);

void ResetBaseTime();

void Respawn_Tick();

#endif // T_BASE_H