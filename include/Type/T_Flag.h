#pragma once

#include "common.h"

#include <Arduino.h>

#include "Core/MyComm.h"
#include "Peripherals/M_LCD.h"


void InitFlag();

void SetFlag(char Id, char c);
void MoveFlag(char c);
void ResetFlag();

void takeRED();
void takeBLU();
void takeFREE();


void tickFlag();