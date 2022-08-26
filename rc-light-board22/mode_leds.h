#ifndef __MODE_LEDS_H
#define __MODE_LEDS_H


#include <Arduino.h>
#include "common.h"


// Device Mode Enum
enum Mode {
  MODE_NORMAL = 100,
  MODE_MANUAL = 200,
  MODE_STROBE = 300,
  MODE_STROBE_PATTERN_1,
  MODE_STROBE_PATTERN_1_ADJUSTMENT,
  MODE_STROBE_PATTERN_2,
  MODE_STROBE_PATTERN_2_ADJUSTMENT,
  MODE_STROBE_PATTERN_3,
  MODE_STROBE_PATTERN_3_ADJUSTMENT,
  MODE_BEACON = 400,
  MODE_BEACON_ON,
  MODE_BEACON_ON_ADJUSTMENT,
  MODE_BEACON_OFF,
  MODE_BEACON_OFF_ADJUSTMENT
};


/* ---------- Public Function Declarations ---------- */
void ModeLEDs_Init();
void ModeLEDs_Set(Mode mode);
void ModeLEDs_SetAll(bool on);


#endif
