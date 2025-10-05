#include "Core/MyConfig.h"

namespace MyConfig {

static bool sInited = false;
static uint16_t sCacheId = 0;

static uint32_t crc32_update(uint32_t crc, uint8_t data) {
  crc ^= data;
  for (int i = 0; i < 8; i++) {
    uint32_t mask = -(crc & 1u);
    crc = (crc >> 1) ^ (0xEDB88320u & mask);
  }
  return crc;
}

static uint32_t crc32_calc(const Config& c) {
  uint32_t crc = 0xFFFFFFFFu;
  const uint8_t* p = reinterpret_cast<const uint8_t*>(&c);
  for (size_t i = 0; i < sizeof(Config) - sizeof(uint32_t); ++i)
    crc = crc32_update(crc, p[i]);
  return ~crc;
}

void begin() {
  if (!sInited) {
    EEPROM.begin(EEPROM_SIZE);
    sInited = true;
  }
}

bool load(Config& out) {
  begin();
  EEPROM.get(EEPROM_BASE, out);
  if (out.version != CFG_VERSION) return false;
  if (out.moduleId == 0) return false;
  return (crc32_calc(out) == out.crc32);
}

void save(const Config& in) {
  begin();
  EEPROM.put(EEPROM_BASE, in);
  EEPROM.commit();
  sCacheId = in.moduleId;
}

void clear() {
  begin();
  Config z{};
  EEPROM.put(EEPROM_BASE, z);
  EEPROM.commit();
  sCacheId = 0;
}

uint16_t getModuleId() {
  if (sCacheId != 0) return sCacheId;
  Config c{};
  if (load(c)) {
    sCacheId = c.moduleId;
    return sCacheId;
  }
  return 0;
}

bool setModuleId(uint16_t id) {
  if (id == 0) return false;
  Config c{};
  c.version  = CFG_VERSION;
  c.moduleId = id;
  c.crc32    = crc32_calc(c);
  save(c);
  return true;
}

bool isProvisioned() {
  return getModuleId() != 0;
}

} // namespace MyConfig
