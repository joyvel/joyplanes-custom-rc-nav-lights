#ifndef __BUTTONS_H
#define __BUTTONS_H


#include <Arduino.h>
#include "common.h"


// Button Event Enum
enum ButtonEvent {
  NO_EVENT = 0,
  BTN_1_SHORT,
  BTN_1_LONG,
  BTN_2_SHORT,
  BTN_2_LONG,
  BTN_3_SHORT,
  BTN_3_LONG,
};


/* ---------- Public Function Declarations ---------- */
void Buttons_Init();
void Buttons_Update();
ButtonEvent Buttons_GetEvent();


#endif
