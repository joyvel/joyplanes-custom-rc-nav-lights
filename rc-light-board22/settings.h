#ifndef __SETTINGS_H
#define __SETTINGS_H


#include <Arduino.h>
#include "common.h"


// Config
#define STROBE_SPACE_TIME_MIN     100   // ms
#define STROBE_SPACE_TIME_MAX     5000  // ms
#define STROBE_SPACE_TIME_DEFAULT 500   // ms
#define BEACON_ON_TIME_MIN        10    // ms
#define BEACON_ON_TIME_MAX        2000  // ms
#define BEACON_ON_TIME_DEFAULT    30    // ms
#define BEACON_OFF_TIME_MIN       10    // ms
#define BEACON_OFF_TIME_MAX       5000  // ms
#define BEACON_OFF_TIME_DEFAULT   1000  // ms


// Settings Structs

struct __attribute__((__packed__ )) StrobeSettings
{
  uint16_t pattern;
  uint16_t spacing;
};

struct __attribute__((__packed__ )) BeaconSettings
{
  uint16_t on_time;
  uint16_t off_time;
};

struct __attribute__((__packed__ )) Settings 
{
  StrobeSettings strobe;
  BeaconSettings beacon;
  bool manual_mode;
  uint16_t crc;
};


/* ---------- Public Function Declarations ---------- */
Settings Settings_Load();
void Settings_Save(Settings &settings);
void Settings_RevertToDefault();


#endif
