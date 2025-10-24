#pragma once

#define LCD

#ifdef LCD

#include <Arduino.h>
#include <Ticker.h>
#include <PA_LCD.h>
#include <utility/Adafruit_MCP23017.h>


// These #defines make it easy to set the backlight color
#define C_BLACK 0x0
#define C_RED 0x1
#define C_YELLOW 0x3
#define C_GREEN 0x2
#define C_TEAL 0x6
#define C_BLUE 0x4
#define C_VIOLET 0x5
#define C_WHITE 0x7



/// @brief Classe représentant le module de type BASE
class M_LCD {

public:
    /// @brief Initialisation du module
    void begin();

    void Clear(char c[]);

    void Write_Msg(const char* L1, const char* L2, const char color);

    void Write_Pop(const char* L1, const char* L2, const char color, const char* t); 

    void SetColor(char c);

    bool AsPopup();

    void setCursor(uint8_t col, uint8_t row);

    void print(const char * text);
    void print(int n);

    void createChar(uint8_t ind, uint8_t charmap[]);

    void write(uint8_t value);

private:

    PA_LCD _lcd = PA_LCD();

    char _backupLine1[17];
    char _backupLine2[17];
    char _backupColor;

    bool _popup = false;

    // Timmer
    Ticker _ticker;

    
    void _endPopup();
};


extern M_LCD lcd; // ← déclaration globale accessible partout

#endif // M_LCD
