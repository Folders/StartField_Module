#pragma once

#include <Arduino.h>
#include <string>
#include <vector>
#include <WiFiUDP.h>


// Enabled all serial communication (put in comment to disabled log mode)
#define LOG

#define DEBUG

/////////////////////////////////   Define options of module   /////////////////////////////////

// Define module number if more then one is used
#define NUMBER 1
#define TERRAIN 'P'


/////////////////////////////////   Define options of module   /////////////////////////////////

#define BASE
#define SAFE
#define FLAG
//#define BOMB

// LCD is used
#define LCD

// Button and lamp used (from I2C)
#define BTN_R
#define BTN_B
#define BTN_O

#define LED_R
#define LED_B

/*
// External materiel used
#define Flash_R
#define Flash_B
#define Buzzer

// Party mode used
#define BASE
#define FLAG
#define BOMB
#define OBJECTIVE

*/


#include "Type/T_Base.h"
#include "Type/T_Safe.h"
#include "Type/T_Flag.h"
#include "Type/T_Bomb.h"


extern char party;

///////////////////////////////////////////////////////////////////////////////////////////////
//                                          Function                                         //
///////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Convert chars to int
/// @param p Text to convert
/// @return Value in integer
int CharToInt(char *p);


/// @brief Convert chars to float
/// @param p Text to convert
/// @return Value in float
float CharToFloat(const char *p);

/// @brief Send more information to the server
extern bool debug;


class MyComm;          // forward declaration
extern MyComm comm;    // objet global fourni par main

class M_LCD;
extern M_LCD lcd;
