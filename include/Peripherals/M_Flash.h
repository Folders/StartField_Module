#pragma once
#include <Arduino.h>

/// @brief Classe représentant le module de type BASE
class M_Flash {

public:
    /// @brief Init flash lamp object
    /// @param color Define flash lamp color
    M_Flash(char color);

    /// @brief Initialisation du module
    void begin(uint8_t buttonPin);

    /// @brief Set flash lampe state
    /// @param value 1: Flashing, 0: stopped
    void setFlash(char value);

private:
    char _color;
    uint8_t _pin;
};
