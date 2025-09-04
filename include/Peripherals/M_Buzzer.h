#pragma once
#include <Arduino.h>
#include <Ticker.h>

/// @brief Classe représentant le module de type BASE
class M_Buzzer {

public:
    /// @brief Initialisation du module
    void begin(uint8_t buttonPin);

    /// @brief Run buzzer during an amount of time
    /// @param time Buzzing time [s]
    void buzz(const char* time);

private:
    uint8_t _pin;


    // Timmer
    Ticker _ticker;
    void _endBuzz();
};
