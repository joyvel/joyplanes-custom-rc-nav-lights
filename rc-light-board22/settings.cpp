#include "settings.h"
#include "debug.h"
#include <EEPROM.h>


#define DEFAULT_SETTINGS {                  \
  .strobe = {                               \
    .pattern = 0,                           \
    .spacing = STROBE_SPACE_TIME_DEFAULT    \
  },                                        \
  .beacon = {                               \
    .on_time = BEACON_ON_TIME_DEFAULT,      \
    .off_time = BEACON_OFF_TIME_DEFAULT     \
  }                                         \
}


/* ---------- Private Function Declarations ---------- */
uint16_t crc16(uint8_t *data, uint16_t len);
uint16_t GetCRC(const Settings &setting);


/* ---------- Public Function Definitions ---------- */

// Load Settings from EEPROM
Settings Settings_Load()
{
  // Load settings from EEPROM
  Settings settings;
  EEPROM.get(0, settings);

  uint16_t crc_calc = GetCRC(settings);
  Debug_Msg("Loading settings (crc_eeprom: %u, crc_calc: %u)", settings.crc, crc_calc);

  // Check CRC on loaded settings
  if (settings.crc != crc_calc)
  {
    Debug_Error("Bad CRC, using default settings.", settings.crc);
    // Default settings
    settings = DEFAULT_SETTINGS;
    settings.crc = GetCRC(settings);
  }

  return settings;
}


// Save Settings to EEPROM
void Settings_Save(Settings &settings)
{
  settings.crc = GetCRC(settings);

  Debug_Msg("Saving settings (crc: %u)", settings.crc);

  EEPROM.put(0, settings);
}


// Rever to default settings
void Settings_RevertToDefault()
{
  Settings settings = DEFAULT_SETTINGS;
  Settings_Save(settings);
}


/* ---------- Private Function Definitions ---------- */
uint16_t GetCRC(const Settings &settings)
{
  return crc16((uint8_t *)&settings, sizeof(Settings)-2);
}


uint16_t crc16(uint8_t* data, uint16_t len)
{
  uint8_t x;
  uint16_t crc = 0xFFFF;
  
  while (len--)
  {
    x = crc >> 8 ^ *data++;
    x ^= x>>4;
    crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
  }
  return crc;
}
