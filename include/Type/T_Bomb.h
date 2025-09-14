#pragma once

#include "common.h"

#include <Arduino.h>

#include "Core/MyComm.h"
#include "Peripherals/M_LCD.h"


void InitBomb();

void SetBomb(char Id, char team, const char Difficulty[]);
void SetPassword(const char Psw[]);
void BombPlanted(const char Psw[], const char Min[], const char Sec[]);
void SetDefuse(const char Psw[]);
void ResetDefuse();
void ResetPopup();
void ResetBomb();


void tickBomb();