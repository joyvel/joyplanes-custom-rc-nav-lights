#include "signal_leds.h"
#include "mode_leds.h"


// Pin Definitions
#define STROBE A1
#define BEACON A2
#define AUX1 A3
#define AUX2 A4


/* ---------- Private Function Declarations ---------- */
void LED_Set(byte pin, bool on);



/* ---------- Private Function Definitions ---------- */

// Set LED output pin
void LED_Set(byte pin, bool on)
{
  digitalWrite(pin, on ? HIGH : LOW);
}


/* ---------- Public Function Definitions ---------- */

// Initialze Signal LEDs
void SignalLEDs_Init()
{
  // Strobe LED
  pinMode(STROBE, OUTPUT);
  LED_Set(STROBE, false);
  // Beacon LED
  pinMode(BEACON, OUTPUT);
  LED_Set(BEACON, false);
  // AUX1 LED
  pinMode(AUX1, OUTPUT);
  LED_Set(AUX1, false);
  // AUX2 LED
  pinMode(AUX2, OUTPUT);
  LED_Set(AUX2, false);
}


// Update LEDs (called on loop)
void SignalLEDs_Update(const SignalLEDsConfig &signal_config)
{
  static uint32_t ms_last = millis();
  uint32_t ms = millis();
  uint32_t d_ms = ms - ms_last;
  
  // Strobe LED
  static uint8_t strobe_i = 0;
  static bool strobe_state = false;
  static int32_t strobe_time_left = 0;

  strobe_time_left -= d_ms;
  
  if (strobe_time_left <= 0)
  {
    strobe_state = !strobe_state;
    if (strobe_state)
    {
      strobe_i = (strobe_i + 1) % STROBE_TIMES_ARR_SIZE;
      strobe_time_left = signal_config.strobe.times.on[strobe_i];
    }
    else
    {
      strobe_time_left = signal_config.strobe.times.off[strobe_i];
    }
  }

  LED_Set(STROBE, strobe_state && signal_config.strobe.enabled);

  // Beacon LED
  static bool beacon_state = false;
  static int32_t beacon_time_left = 0;

  beacon_time_left -= d_ms;

  if (beacon_time_left <= 0)
  {
    beacon_state = !beacon_state;
    if (beacon_state)
    {
      beacon_time_left = signal_config.beacon.on_time;
    }
    else
    {
      beacon_time_left = signal_config.beacon.off_time;
    }
  }
  
  LED_Set(BEACON, beacon_state && signal_config.beacon.enabled);


  // AUX1 LED
  LED_Set(AUX1, signal_config.aux1_on);
  
  // AUX2 LED
  LED_Set(AUX2, signal_config.aux2_on);

  // Track time
  ms_last = ms;
}
