#pragma once
#include <Arduino.h>
#include <EEPROM.h>

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 8
#endif

namespace MyConfig {

struct Config {
  uint16_t version;
  uint16_t moduleId;
  uint32_t crc32;
};

constexpr uint16_t CFG_VERSION = 1;
constexpr int EEPROM_BASE = 0;

void begin();                 // à appeler au boot (setup)
bool load(Config& out);       // true si config valide
void save(const Config& in);  // écrit + commit
void clear();                 // remet à blanc

// Helpers centrés sur l'ID
uint16_t getModuleId();       // 0 si non provisionné
bool setModuleId(uint16_t id);// sauve, true si OK
bool isProvisioned();         // (getModuleId()!=0)

} // namespace MyConfig
