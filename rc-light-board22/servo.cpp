#include <Arduino.h>
#include "servo.h"


// Pin Definitions
// servo signal pins need to stay in D0-D7 otherwise pci scheme also needs to be updated (see PCI setup code and ISRs)
#define PWM1_PIN 5
#define PWM2_PIN 6


// Config
#define PWM_INACTIVE_TIMEOUT 25 // ms
#define SW_MIDDLE_POSITION_MAX 1800
#define SW_MIDDLE_POSITION_MIN 1200


/* ---------- Private Function Declarations ---------- */
bool Servo_GetPositionSW(uint8_t i, uint8_t *sw_pos, bool invert=false);


/* ---------- Servo Signal Class ---------- */
// handles PWM signal and pulse duration detection
class ServoSignal {
  public:
  
    // Constructor
    ServoSignal(byte pin) :
    _pin(pin)
    {
      _last_pin_state = 1;
      _last_fall_time = 0;
      _last_rise_time = 0;
    }

    // Initialize servo signal pwm pins
    void Init()
    {
      pinMode(_pin, INPUT_PULLUP);
      *digitalPinToPCMSK(_pin) |= bit(digitalPinToPCMSKbit(_pin)); // enable pin
      PCIFR |= bit(digitalPinToPCICRbit(_pin)); // clear any outstanding interrupt
      PCICR |= bit(digitalPinToPCICRbit(_pin)); // enable interrupt for the group
    }

    // Call from pin change interrupt
    void Update()
    {
      uint32_t us = micros();
      
      bool cur_pin_state = PIND & bit(_pin);
      if (cur_pin_state && !_last_pin_state)
      {
        _last_period = us - _last_rise_time;
        _last_rise_time = us;
      }
      else if (!cur_pin_state && _last_pin_state)
      {
        _last_pw = us - _last_rise_time;
        _last_fall_time = us; 
      }

      _last_pin_state = cur_pin_state;
    }

    // Get the current pulse width of the signal
    // Returns true if signal is active
    bool GetPulseWidth(uint16_t *pw)
    {
      uint32_t us = micros();
      uint32_t last_change = max(_last_rise_time, _last_fall_time);
      
      // Return false if signal is inactive
      if (us - last_change > PWM_INACTIVE_TIMEOUT*1000)
      {
        return false;
      }

      // Set pulse width and return true if signal is active
      *pw = _last_pw;
      return true;
    }
    
  private:
    const byte _pin;
    bool _last_pin_state;
    uint32_t _last_fall_time;
    uint32_t _last_rise_time;
    uint16_t _last_pw;
    uint16_t _last_period;
};


// Servo Signals Array
#define NUM_PWMS (sizeof(pwm)/sizeof(ServoSignal))
ServoSignal pwm[] = {
  ServoSignal(PWM1_PIN),
  ServoSignal(PWM2_PIN)
};


/* ---------- Interrupts ---------- */

// Pin change interrupt for PWM signal reading
ISR(PCINT2_vect)
{
  for (uint8_t i=0; i<NUM_PWMS; i++)
  {
    pwm[i].Update();
  }
}


/* ---------- Public Function Definitions ---------- */

// Initialize Servo Signal Pins
void Servo_Init()
{
  for (uint8_t i=0; i<NUM_PWMS; i++)
  {
    pwm[i].Init();
  } 
}


// Get current switch 1 position
bool Servo_GetPositionSW1(uint8_t *sw_pos)
{
  return Servo_GetPositionSW(0, sw_pos);
}


// Get current Switch 2 position
bool Servo_GetPositionSW2(uint8_t *sw_pos)
{
  return Servo_GetPositionSW(1, sw_pos, true);
}


// Get signal status and pulse width (us) for signal 1
bool Servo_GetSignal1(uint16_t *pulse_width)
{
  return pwm[0].GetPulseWidth(pulse_width);
}


// Get signal status and pulse width (us) for signal 2
bool Servo_GetSignal2(uint16_t *pulse_width)
{
  return pwm[1].GetPulseWidth(pulse_width);
}


/* ---------- Private Function Definitions ---------- */

// Get current switch position
bool Servo_GetPositionSW(uint8_t i, uint8_t *sw_pos, bool invert)
{
  uint16_t pw = 0;
  bool signal_active = pwm[i].GetPulseWidth(&pw);
  if (!signal_active)
  {
    return false;
  }

  *sw_pos = (pw < SW_MIDDLE_POSITION_MIN) ? 0 : (pw <= SW_MIDDLE_POSITION_MAX) ? 1 : 2;
  if (invert)
  {
    *sw_pos = 2 - *sw_pos;
  }
  
  return true;
}
