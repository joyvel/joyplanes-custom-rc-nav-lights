#include "mode_leds.h"


// Pin Definitions
#define MD1 12
#define MD2 13
#define PR1 2
#define PR2 4


// Config
#define PR_LED_BLINK_PERIOD 500 // ms


/* ---------- Private Function Definitions ---------- */

// Set Mode LEDs
void MD_Set(bool md1, bool md2)
{
  digitalWrite(MD1, md1 ? HIGH : LOW);
  digitalWrite(MD2, md2 ? HIGH : LOW);
}


// Set Parameter LEDs
void PR_Set(bool pr1, bool pr2)
{
  digitalWrite(PR1, pr1 ? HIGH : LOW);
  digitalWrite(PR2, pr2 ? HIGH : LOW);
}


/* ---------- Public Function Definitions ---------- */

// Initalize Mode LEDs
void ModeLEDs_Init()
{
  // Mode LEDS MD1, MD2
  pinMode(MD1, OUTPUT);
  digitalWrite(MD1, HIGH);
  pinMode(MD2, OUTPUT);
  digitalWrite(MD2, HIGH);
  // Mode Parameter LEDS PR1, PR2
  pinMode(PR1, OUTPUT);
  digitalWrite(PR1, HIGH);
  pinMode(PR2, OUTPUT);
  digitalWrite(PR2, HIGH);
}


// Set Mode LEDs for current mode
void ModeLEDs_Set(Mode mode)
{
  bool pr_blink = (millis() / PR_LED_BLINK_PERIOD) % 2;
  
  // Set Mode LEDs
  if (mode >= MODE_NORMAL && mode < MODE_NORMAL + 100)
  {
    MD_Set(0, 0);
    PR_Set(0, 0);
  }
  else if (mode >= MODE_MANUAL && mode < MODE_MANUAL + 100)
  {
    MD_Set(1, 1);
    PR_Set(0, 0);
  }
  else if (mode >= MODE_STROBE && mode < MODE_STROBE + 100)
  {
    MD_Set(1, 0);
    if      (mode == MODE_STROBE_PATTERN_1)             { PR_Set(1, 0); }
    else if (mode == MODE_STROBE_PATTERN_2)             { PR_Set(0, 1); }
    else if (mode == MODE_STROBE_PATTERN_3)             { PR_Set(1, 1); }
    else if (mode == MODE_STROBE_PATTERN_1_ADJUSTMENT)  { PR_Set(pr_blink, 0); }
    else if (mode == MODE_STROBE_PATTERN_2_ADJUSTMENT)  { PR_Set(0, pr_blink); }
    else if (mode == MODE_STROBE_PATTERN_3_ADJUSTMENT)  { PR_Set(pr_blink, pr_blink); }
    else                                                { PR_Set(0, 0); }
  }
  else if (mode >= MODE_BEACON && mode < MODE_BEACON + 100)
  {
    MD_Set(0, 1);
    if      (mode == MODE_BEACON_ON)              { PR_Set(1, 0); }
    else if (mode == MODE_BEACON_OFF)             { PR_Set(0, 1); }
    else if (mode == MODE_BEACON_ON_ADJUSTMENT)   { PR_Set(pr_blink, 0); }
    else if (mode == MODE_BEACON_OFF_ADJUSTMENT)  { PR_Set(0, pr_blink); }
    else                                          { PR_Set(0, 0); }
  }
}


// Set all mode LEDs
// used to override normal display
void ModeLEDs_SetAll(bool on)
{
  MD_Set(on, on);
  PR_Set(on, on);
}
