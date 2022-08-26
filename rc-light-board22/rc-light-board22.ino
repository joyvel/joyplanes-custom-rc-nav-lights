
#include <Arduino.h>
#include "common.h"
#include "settings.h"
#include "debug.h"
#include "buttons.h"
#include "servo.h"
#include "mode_leds.h"
#include "signal_leds.h"


#define POT_PIN A0


#define INACTIVITY_TIMEOUT 60000 // ms


#define SIGNAL_SW 1
#define AUX_SW 2

//#define SW1_FUNCTION SIGNAL_SW
#define SW1_FUNCTION AUX_SW

//#define PRINT_INPUTS


/* ---------- Global variables ---------- */
// Mode for state machine
Mode _mode = MODE_NORMAL;
Mode _mode_next = MODE_NORMAL;
Mode _mode_last = MODE_NORMAL;
// Device settings
Settings _settings;
// Last button press time for inactivity timeout
static uint32_t _last_button_activity = 0;
static uint32_t _last_pot_activity = 0;


/* ---------- Private Function Declarations ---------- */
void HandleButton1(bool long_press=false);
void HandleButton2(bool long_press=false);
void HandleButton3(bool long_press=false);
void HandleButtonEvent(ButtonEvent evt, uint32_t ms);
uint16_t ScaledParam(uint16_t raw, uint16_t pmin, uint16_t pmax);


/* ---------- Arduino Setup ---------- */
void setup()
{
  Debug_Init();
  Debug_Msg("Setup...");
  
  Debug_Msg("Loading Settings...");
  _settings = Settings_Load();

  // Initial mode
  _mode_next = _mode = _mode_last = _settings.manual_mode ? MODE_MANUAL : MODE_NORMAL;

  Debug_Msg("Buttons Init...");
  Buttons_Init();
  
  Debug_Msg("Servo Init...");
  Servo_Init();
  
  Debug_Msg("Mode LEDs Init...");
  ModeLEDs_Init();
  
  Debug_Msg("Signal LEDs Init...");
  SignalLEDs_Init();

  Debug_Msg("Done.");
}


/* ---------- Arduino Loop ---------- */
void loop()
{
  /* ---------- Loop Start ---------- */
  
  // Loop start time
  uint32_t ms = millis();
  
  // Update mode
  _mode_last = _mode;
  _mode = _mode_next;
  if (_mode != _mode_last)
  {
    Debug_Verbose("mode change: (%u -> %u)", _mode_last, _mode);
  }


  /* ---------- Inputs ---------- */
  
  // Switch Positions (Servo Signals)
  uint8_t sw1_pos=0, sw2_pos=0;
  bool sw1_active = Servo_GetPositionSW1(&sw1_pos);
  bool sw2_active = Servo_GetPositionSW2(&sw2_pos);
  
  // Potentiometer
  uint32_t raw = analogRead(POT_PIN);
  static uint32_t raw_last = 0;
  static uint32_t pot_val = 0;
  if (abs(raw - raw_last) > 2)
  {
    // better scaling for adjusting lower end than with linear
    pot_val = raw * raw / 1023;
    raw_last = raw;
    _last_pot_activity = ms;
  }
  
  // Buttons
  Buttons_Update(); // this is called here in addition to from pin change interrupts in order to not wait for btn release to trigger long hold presses
  ButtonEvent btn_evt = Buttons_GetEvent();

#ifdef PRINT_INPUTS
  static uint32_t last_input_print = 0;
  const char *active[] = {"INACTIVE","ACTIVE"};
  if (ms - last_input_print > 1000)
  {
    Debug_Msg("=== Inputs ===");
    Debug_Msg("SW1: %s, %u", active[sw1_active], sw1_pos);
    Debug_Msg("SW2: %s, %u", active[sw2_active], sw2_pos);
    Debug_Msg("POT: raw: %u, sqr: %u (0-1023)", (uint16_t)raw, (uint16_t)pot_val);
    last_input_print = ms;
  }
#endif

  
  /* ---------- Mode Logic ---------- */
  switch (_mode)
  {
    case MODE_NORMAL:
      _settings.manual_mode = false;
      break;
    case MODE_MANUAL:
      _settings.manual_mode = true;
      break;
    case MODE_STROBE:
      _mode_next = MODE_STROBE_PATTERN_1;
      break;
    case MODE_STROBE_PATTERN_1_ADJUSTMENT:
      _settings.strobe.spacing = ScaledParam(pot_val, STROBE_SPACE_TIME_MIN, STROBE_SPACE_TIME_MAX);
    case MODE_STROBE_PATTERN_1:
      _settings.strobe.pattern = 0;
      break;
    case MODE_STROBE_PATTERN_2_ADJUSTMENT:
      _settings.strobe.spacing = ScaledParam(pot_val, STROBE_SPACE_TIME_MIN, STROBE_SPACE_TIME_MAX);
    case MODE_STROBE_PATTERN_2:
      _settings.strobe.pattern = 1;
      break;
    case MODE_STROBE_PATTERN_3_ADJUSTMENT:
      _settings.strobe.spacing = ScaledParam(pot_val, STROBE_SPACE_TIME_MIN, STROBE_SPACE_TIME_MAX);
    case MODE_STROBE_PATTERN_3:
      _settings.strobe.pattern = 2;
      break;
    case MODE_BEACON:
      _mode_next = MODE_BEACON_ON;
      break;
    case MODE_BEACON_ON:
      break;
    case MODE_BEACON_ON_ADJUSTMENT:
      _settings.beacon.on_time = ScaledParam(pot_val, BEACON_ON_TIME_MIN, BEACON_ON_TIME_MAX);
      break;
    case MODE_BEACON_OFF:
      break;
    case MODE_BEACON_OFF_ADJUSTMENT:
      _settings.beacon.off_time = ScaledParam(pot_val, BEACON_OFF_TIME_MIN, BEACON_OFF_TIME_MAX);
      break;
    default:
      Debug_Error("Invalid Mode, set to Normal Mode.");
      _mode_next = MODE_NORMAL;
  }
  
  // Handle Button Events
  if (btn_evt != NO_EVENT) {
    HandleButtonEvent(btn_evt, ms);
  }

  // Handle Inactivity
  if (ms - max(_last_button_activity, _last_pot_activity) > INACTIVITY_TIMEOUT)
  {
    if (_mode != MODE_MANUAL)
    {
      Debug_Msg("INACTIVITY: going to normal mode.");
      _mode_next = MODE_NORMAL;
    }
  }


  /* ---------- Outputs ---------- */

  // Mode LEDs
  ModeLEDs_Set(_mode);

  // Switches
#if SW1_FUNCTION == SIGNAL_SW
  const uint8_t signal_sw_pos = sw1_pos;
  const bool signal_sw_active = sw1_active;
  const uint8_t aux_sw_pos = sw2_pos;
  const bool aux_sw_active = sw2_active;
#endif
#if SW1_FUNCTION == AUX_SW
  const uint8_t signal_sw_pos = sw2_pos;
  const bool signal_sw_active = sw2_active;
  const uint8_t aux_sw_pos = sw1_pos;
  const bool aux_sw_active = sw1_active;
#endif

  // Signal LEDs 
  SignalLEDs_Update({
    .strobe = {
      .enabled = (
           (_mode == MODE_NORMAL && (signal_sw_pos >= 1 || !signal_sw_active))
        || (_mode == MODE_MANUAL)
        || (_mode >= MODE_STROBE && _mode < MODE_STROBE + 100)
       ),
      .times = StrobePattern(_settings.strobe.pattern, _settings.strobe.spacing)   
    },
    .beacon = {
      .enabled = (
           (_mode == MODE_NORMAL && (signal_sw_pos == 2 || !signal_sw_active))
        || (_mode == MODE_MANUAL)
        || (_mode >= MODE_BEACON && _mode < MODE_BEACON + 100)
       ),
      .on_time = _settings.beacon.on_time,
      .off_time = _settings.beacon.off_time,
    },
    .aux1_on = (
         (_mode == MODE_NORMAL && (aux_sw_pos >= 1))
      || (_mode == MODE_MANUAL)
    ),
    .aux2_on = (
         (_mode == MODE_NORMAL && aux_sw_active && aux_sw_pos == 2)
    )
  });


  /* ---------- Loop End ---------- */

  // Delay until next cycle
  const uint8_t cycle_time_ms = 5;
  uint32_t this_loop_time = millis() - ms;
  if (this_loop_time < cycle_time_ms)
  {
    delay(cycle_time_ms - this_loop_time);
  }
}


// Handle Buttopn Event
void HandleButtonEvent(ButtonEvent evt, uint32_t ms)
{
  _last_button_activity = ms;

  Debug_Verbose("button event: %d", evt);
  
  switch (evt)
  {
    case BTN_1_SHORT: HandleButton1();     break;
    case BTN_1_LONG:  HandleButton1(true); break;
    case BTN_2_SHORT: HandleButton2();     break;
    case BTN_2_LONG:  HandleButton2(true); break;
    case BTN_3_SHORT: HandleButton3();     break;
    case BTN_3_LONG:  HandleButton3(true); break;
    case NO_EVENT:
    default: break;
  }
}


// Handle Button 1 Press
void HandleButton1(bool long_press)
{
  switch (_mode)
  {
    case MODE_NORMAL:
      _mode_next = long_press ? MODE_MANUAL : MODE_STROBE;
      break;
    case MODE_MANUAL:
      _mode_next = long_press ? MODE_NORMAL : MODE_STROBE;
      break;
    case MODE_STROBE:
    case MODE_STROBE_PATTERN_1:
    case MODE_STROBE_PATTERN_1_ADJUSTMENT:
    case MODE_STROBE_PATTERN_2:
    case MODE_STROBE_PATTERN_2_ADJUSTMENT:
    case MODE_STROBE_PATTERN_3:
    case MODE_STROBE_PATTERN_3_ADJUSTMENT:
      _mode_next = long_press ? MODE_NORMAL : MODE_BEACON;
      break;
    case MODE_BEACON:
    case MODE_BEACON_ON:
    case MODE_BEACON_ON_ADJUSTMENT:
    case MODE_BEACON_OFF:
    case MODE_BEACON_OFF_ADJUSTMENT:
      _mode_next = long_press ? MODE_NORMAL : MODE_STROBE;
      break;
    default:
      Debug_Error("Invalid Mode, set to Normal Mode.");
      _mode_next = MODE_NORMAL;
  }
}


// Handle Button 2 Press
void HandleButton2(bool long_press)
{
  switch (_mode)
  {
    case MODE_NORMAL:
    case MODE_MANUAL:
    
    // Strobe Config Mode
    case MODE_STROBE:
      break;
    case MODE_STROBE_PATTERN_1:
      _mode_next = long_press ? MODE_STROBE_PATTERN_1_ADJUSTMENT : MODE_STROBE_PATTERN_2;
      break;
    case MODE_STROBE_PATTERN_1_ADJUSTMENT:
      _mode_next = MODE_STROBE_PATTERN_1;
      break;
    case MODE_STROBE_PATTERN_2:
      _mode_next = long_press ? MODE_STROBE_PATTERN_2_ADJUSTMENT : MODE_STROBE_PATTERN_3;
      break;
    case MODE_STROBE_PATTERN_2_ADJUSTMENT:
      _mode_next = MODE_STROBE_PATTERN_2;
      break;
    case MODE_STROBE_PATTERN_3:
      _mode_next = long_press ? MODE_STROBE_PATTERN_3_ADJUSTMENT : MODE_STROBE_PATTERN_1;
      break;
    case MODE_STROBE_PATTERN_3_ADJUSTMENT:
      _mode_next = MODE_STROBE_PATTERN_3;
      break;

    // Beacon Config Mode
    case MODE_BEACON:
      break;
    case MODE_BEACON_ON:
      _mode_next = long_press ? MODE_BEACON_ON_ADJUSTMENT : MODE_BEACON_OFF;
      break;
    case MODE_BEACON_ON_ADJUSTMENT:
      _mode_next = MODE_BEACON_ON;
      break;
    case MODE_BEACON_OFF:
      _mode_next = long_press ? MODE_BEACON_OFF_ADJUSTMENT : MODE_BEACON_ON;
      break;
    case MODE_BEACON_OFF_ADJUSTMENT:
      _mode_next = MODE_BEACON_OFF;
      break;
    default:
      Debug_Error("Invalid Mode, set to Normal Mode.");
      _mode_next = MODE_NORMAL;
  } 
}


// Handle Button 3 Press
void HandleButton3(bool long_press)
{
  if (long_press)
  {
    Settings_RevertToDefault();
    _settings = Settings_Load();
    for (uint8_t i=1; i<=6; i++)
    {
      delay(1000);
      ModeLEDs_SetAll(i%2);
    }
  }
  else
  {
    Settings_Save(_settings);
    for (uint8_t i=1; i<=6; i++)
    {
      delay(100);
      ModeLEDs_SetAll(i%2);
    }
  }
}


// Get scaled parameter from scaling values and raw pot value
uint16_t ScaledParam(uint16_t raw, uint16_t pmin, uint16_t pmax)
{
  uint32_t _pmin = pmin;
  uint32_t _pmax = pmax;
  uint32_t _raw = raw;

  uint32_t _scaled = (_pmax - _pmin) * _raw / 1023 + _pmin;
  uint16_t scaled = _scaled;

  return scaled;
}
