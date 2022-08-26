#ifndef __SIGNAL_LEDS_H
#define __SIGNAL_LEDS_H


#include <Arduino.h>
#include "common.h"


#define STROBE_TIMES_ARR_SIZE 3


// Strobe Pattern Structs

struct StrobeTimes {
  uint16_t on[STROBE_TIMES_ARR_SIZE];
  uint16_t off[STROBE_TIMES_ARR_SIZE];
};

struct StrobeConfig
{
  bool enabled;
  StrobeTimes times;
};

struct BeaconConfig
{
  bool enabled;
  uint16_t on_time;
  uint16_t off_time;
};

struct SignalLEDsConfig
{
  StrobeConfig strobe;
  BeaconConfig beacon;
  bool aux1_on;
  bool aux2_on;
};


// Get strobe pattern for specified pattern and spaciing
static StrobeTimes StrobePattern(byte i, uint16_t spacing)
{
  StrobeTimes t;
  switch (i)
  {
    case 2:  return { .on = {50, 50, 50}, .off = {250, 900, spacing} };
    case 1:  return { .on = {50, 50, 50}, .off = {250, 250, spacing} };
    case 0:
    default: return { .on = {50, 50,   0}, .off = {250, spacing, 0} };
  }
}


/* ---------- Public Function Declarations ---------- */
void SignalLEDs_Init();
void SignalLEDs_Update(const SignalLEDsConfig &signal_config);


#endif
