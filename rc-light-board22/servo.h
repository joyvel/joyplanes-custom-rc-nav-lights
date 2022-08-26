#ifndef __SERVO_H
#define __SERVO_H


#include <Arduino.h>
#include "common.h"


/* ---------- Public Function Declarations ---------- */
void Servo_Init();
bool Servo_GetPositionSW1(uint8_t *sw_pos);
bool Servo_GetPositionSW2(uint8_t *sw_pos);

// Just here for testing if needed
bool Servo_GetSignal1(uint16_t *pulse_width);
bool Servo_GetSignal2(uint16_t *pulse_width);


#endif
