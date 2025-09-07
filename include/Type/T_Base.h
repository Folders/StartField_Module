#pragma once

#include "common.h"

#include <Arduino.h>
#include <Ticker.h>

#include "Core/MyComm.h"
#include "Peripherals/M_LCD.h"


void SetBase(char Id, char Team, char Type, char Spawn);

void SetBaseTime(const char Min[], const char Sec[]);

void ResetBaseTime();

void Respawn_Tick();