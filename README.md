# Joyplanes light navigation system for RC airplanes and UAV's in general
Fatures: 
1. Completely customizable timing of flash patterns for independent channels.
2. Control your lights from your transmitter, switch them on or off, change modes, etc.
3. Change modes and customize timing by using built-in buttons and potentiometer.
4. x4 light outputs, strobe, beacon, auxiliary 1 and auxiliary 2, and 2 PWM inputs from the receiver.
5. Opensource code for you to furder customize the system to specific needs.

# Getting Started
1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Open `rc-light-board/rc-light-board.ino` from Arduino IDE
3. Select `Tools` -> `Board` -> `Arduino Uno`
4. Plug board in
5. Click `Upload` (Right arrow icon below toolbar) to flash code to ATMEGA328P

# Pins
| Name   | Type        | Arduino Pin | Chip Pin |
| ------ | ----------- | ----------- | -------- |
| POT    | analog in   | A0          | PC0      |
| SW1    | pwm in      | 5           | PD5      |
| SW2    | pwm in      | 6           | PD6      |
| BTN1   | digital in  | 8           | PB0      |
| BTN2   | digital in  | 9           | PB1      |
| BTN3   | digital in  | 10          | PB2      |
| STROBE | digital out | A1          | PC1      |
| BEACON | digital out | A2          | PC2      |
| AUX1   | digital out | A3          | PC3      |
| AUX2   | digital out | A4          | PC4      |
| MD1    | digital out | 12          | PB4      |
| MD2    | digital out | 13          | PB5      |
| PR1    | digital out | 2           | PD0      |
| PR2    | digital out | 4           | PD2      |

# Functional Overview
The board has 4 output pins for external LEDs that can be configured in various modes/flashing sequences.
| LED    | Description                                                                                            |
| ------ | ------------------------------------------------------------------------------------------------------ |
| Strobe | 3 selectable flashing modes with adjustable timing via button menu<br>Enabled/Disabled via RC switches |
| Beacon | Adjustable flashing timing via button menu<br>Enabled/Disabled via RC switches                         |
| Aux 1  | On/Off via RC switches                                                                                 |
| Aux 2  | On/Off via RC switches                                                                                 |

The board has 3 buttons for selecting modes and 1 potentiometer/knob for adjusting parameters.
| Input | Description                                                            |
| ----- | ---------------------------------------------------------------------- |
| Btn 1 | Switches between Strobe and Beacon config modes                        |
| Btn 2 | Controls adjustment of parameters for Strobe and Beacon configurations |
| Btn 3 | Saves current configuration to EEPROM while in config mode             |
| Pot   | Adjusts currently selected parameter in config mode                    |

# Operation
## Modes
The board has 4 modes:
| Mode                  | Description |
| --------------------- | ----------- |
| 1. Normal Mode        | Beacon, Strobe, Aux1, and Aux2 outputs are controlled according the board's configuration and RC switch signals. |
| 2. Manual Mode        | Beacon and Strobe are controlled according to the board's configuration.  Aux1 is ON. Aux2 is OFF. |
| 3. Strobe Config Mode | Strobe configuration is adjustable from this mode.<br>While in this mode Strobe flashes according to config as it's adjusted.  All other outputs are OFF. |
| 4. Beacon Config Mode | Beacon configuration is adjustable form this mode.<br>While in this mode Beacon flashes according to config as it's adjusted.  All other outputs are OFF. |

### Normal Mode
In Normal Mode, the LED outputs will be controlled by switch signals wired to the board.  If neither switch signal is wired then it's the same as being in Manual Mode.

| SW1 Position | Result                              |
| ------------ | ----------------------------------- |
| 0            | Strobe Disabled<br/>Beacon Disabled |
| 1            | Strobe Enabled<br/>Beacon Disabled  |
| 2            | Strobe Enabled<br/>Beacon Enabled   |
| No Signal    | Strobe Enabled<br/>Beacon Enabled   |

| SW2 Position | Result                |
| ------------ | --------------------- |
| 0            | AUX1 OFF<br/>AUX2 OFF |
| 1            | AUX1 ON<br/>AUX2 OFF  |
| 2            | AUX1 ON<br/>AUX2 ON   |
| No Signal    | AUX1 ON<br/>AUX2 OFF  |

- A short press (< 1 second) of Btn 1 will switch to Strobe Config Mode
- A long press (>= second) of Btn 1 will switch to Manual Mode

### Manual Mode
In Manual Mode, the Strobe and Beacon are enabled, Aux 1 is ON, and Aux 2 is OFF.  In this mode any RC switch signals wired to the board are ignored.
- A short press (< 1 second) of Btn 1 will switch to Strobe Config Mode
- A long press (>= second) of Btn 1 will switch back to Normal Mode

### Strobe Config Mode
While in Strobe Config Mode you can use Btn 2 and the Pot to select from the 3 Strobe modes and adjust the timing of each.

At any point in Strobe Config mode you can press Btn 3 to save any changes you've made in Strobe Config Mode or press Btn 1 to discard changes and switch to Beacon Config Mode.

After 60 seconds of inactivity in Strobe Config Mode, the board will switch to Normal Mode without saving any unsaved changes.

 - A short press (< 1 second) of Btn 2 will switch between the 3 strobe flashing pattern selections.  
 - A long press (>= 1 second) of Btn 2 will enter spacing adjustment mode for the currently selected flashing pattern.  While in spacing adjustment mode, the potentiometer can be turned to adjust the spacing time between it's min and max values.  While in spacing adjustment mode, pressing Btn 2 again (short or long will return user back to the base Strobe Config Mode).

### Beacon Config Mode
While in Beacon Config Mode you can use Btn 2 and the Pot to adjust the pulse timing for the beacon LED.

At any point in Beacon Config mode you can press Btn 3 to save any changes you've made in Beacon Config Mode or press Btn 1 to discard changes and switch to Strobe Config Mode.

After 60 seconds of inactivity in Beacon Config Mode, the board will switch to Normal Mode without saving any unsaved changes.

 - A short press (< 1 second) of Btn 2 will switch between selection of ON and OFF timing.
 - A long press (>= 1 second) of Btn 2 will enter timing adjustment mode for the currently selected time (ON or OFF).  While in timing adjustment mode, pressing Btn 2 again (short or long will return user back to the base Beacon Config Mode).

### Saving
 - At any point a short press (< 10 seconds) of Btn 3 will save the current device configuration to EEPROM.  All mode LEDs will do 3 quick flashes when settings are saved.
 - At any point a long press (>= 10 seconds) of Btn 3 will revert the device to its default device settings.  All mode LEDs will do 3 slow flashes when device is reset.

### Mode LEDs
Two mode LEDs (`MD1`, `MD2`) indicate what mode the board is currently in.
| MD1 | MD2 | Mode          |
| --- | --- | ------------- |
| OFF | OFF | Normal Mode   |
| ON  | ON  | Manual Mode   |
| ON  | OFF | Strobe Config |
| OFF | ON  | Beacon Config |

Two parameter LEDs (`PR1`, `PR2`) indicate what parameter is currently selected for adjustment (if any).  This is only applicable in Strobe and Beacon Config modes
| Mode          | PR1 | PR2 | Selected Parameter            |
| ------------- | --- | --- | ----------------------------- |
| Strobe Config | OFF | OFF | No parameter is selected      |
| Strobe Config | ON  | OFF | Strobe pattern 1 spacing time |
| Strobe Config | OFF | ON  | Strobe pattern 2 spacing time |
| Strobe Config | ON  | ON  | Strobe pattern 3 spacing time |
| Beacon Config | OFF | OFF | No parameter is selected      |
| Beacon Config | ON  | OFF | Beacon pulse ON time          |
| Beacon Config | OFF | ON  | Beacon Pulse OFF time         |
| Beacon Config | ON  | ON  | NA                            |

## Hardcoded Config Parameters
| Parameter                 | Value | Unit |
| ------------------------- | ----- | ---- |
| STROBE_SPACE_TIME_MIN     | 100   | ms   |
| STROBE_SPACE_TIME_MAX     | 5000  | ms   |
| STROBE_SPACE_TIME_DEFAULT | 500   | ms   |
| BEACON_ON_TIME_MIN        | 10    | ms   |
| BEACON_ON_TIME_MAX        | 2000  | ms   |
| BEACON_ON_TIME_DEFAULT    | 30    | ms   |
| BEACON_OFF_TIME_MIN       | 10    | ms   |
| BEACON_OFF_TIME_MAX       | 5000  | ms   |
| BEACON_OFF_TIME_DEFAULT   | 1000  | ms   |
