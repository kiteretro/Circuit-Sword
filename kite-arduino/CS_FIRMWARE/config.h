/*
 * This file originates from Kite's Circuit Sword control board project.
 * Author: Kite/Geebles/kiteretro (Giles Burgess)
 * 
 * THIS HEADER MUST REMAIN WITH THIS FILE AT ALL TIMES
 *
 * This firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this repo. If not, see <http://www.gnu.org/licenses/>.
 */

#include "defs.h"

// Board models
#define MODEL MODEL_CSORIGINAL
//#define MODEL MODEL_CSADVANCE
//#define MODEL MODEL_CSPOCKET

// Board versions - YOU DO NOT NEED TO CHANGE THIS
#define HARDWARE HARDWARE_CSO_V1_2A

// Options
//#define DEBUG //Print out a load of debug messages. YOU NEED THE SERIAL MONITOR ALWAYS OPEN OTHERWISE THE BUFFER FILLS UP
//#define NOMODE //Disable the use of the 'mode' button completely
//#define CONFIG_WIFI_DEFAULT_OFF
//#define USE_OLD_BUTTON_COMBOS //Changes UP/DOWN to do brightness instead
//#define USE_VOLUME_DIGITAL // Use CC1/CC2 or the rocker switch for volume (CC1/CC2)
//#define USE_ALT_PINS_VOLUME_DIGITAL // Alternative digital volume pins (AL1/AL2)
#define USE_AUTODIM

// Default Hardware pins
#define PIN_LCD_CS 8
#define PIN_LCD_RESET 6
#define PIN_LCD_SDO 16
#define PIN_LCD_CLK 15
#define PIN_VOLT A2
#define PIN_BTN_MODE 7
#define PIN_INT 12 //TCA9555 interrupt
#define PIN_LED 5
#define PIN_BL 11
#define PIN_AUD 13
#define PIN_JOY1_X A9  //D9
#define PIN_JOY1_Y A10 //D10
#define PIN_VOL_D_ALT_UP 1 //TX0
#define PIN_VOL_D_ALT_DOWN 0 //RX0

// Custom hardware bits
#if HARDWARE == HARDWARE_CSO_V1_1E
#elif HARDWARE == HARDWARE_CSO_V1_2A
#else
#error "ERROR: HARDWARE not defined at the top of config.h, please set it!"
#endif

// Model check and pin overrides
#if MODEL == MODEL_SAIO
#error "ERROR: MODEL SAIO NOT SUPPORTED (YET)"

#elif MODEL == MODEL_CSORIGINAL
#define USE_JOYSTICK_1
#define USE_JOYSTICK_2
#define USE_VOLUME_ANALOG
#define USE_ALT_PINS_VOLUME_DIGITAL
#define LCD LCD_35_640_54P
#define BTN_UP     15
#define BTN_DOWN   13
#define BTN_LEFT   8
#define BTN_RIGHT  14
#define BTN_A      1
#define BTN_B      0
#define BTN_X      3
#define BTN_Y      2
#define BTN_START  10
#define BTN_SELECT 9
#define BTN_L1     4
#define BTN_L2     5
#define BTN_R1     6
#define BTN_R2     7
#define BTN_C1     12
#define BTN_C2     11
#define PIN_JOY2_X A0
#define PIN_JOY2_Y A1
#define PIN_A_VOL  A6

#elif MODEL == MODEL_CSADVANCE
#define USE_JOYSTICK_1
#define USE_VOLUME_ANALOG
#define LCD LCD_32_800
#define BTN_UP     9
#define BTN_DOWN   14
#define BTN_LEFT   11
#define BTN_RIGHT  15
#define BTN_A      3
#define BTN_B      0
#define BTN_X      6
#define BTN_Y      4
#define BTN_START  13
#define BTN_SELECT 12
#define BTN_L1     8
#define BTN_L2     10
#define BTN_R1     5
#define BTN_R2     7
#define BTN_C1     2
#define BTN_C2     1
#define PIN_JOY2_X A4
#define PIN_JOY2_Y A5

#elif MODEL == MODEL_CSPOCKET
#define USE_VOLUME_DIGITAL
#define LCD LCD_26_320
#define BTN_UP     13
#define BTN_DOWN   15
#define BTN_LEFT   10
#define BTN_RIGHT  14
#define BTN_A      3
#define BTN_B      0
#define BTN_X      2
#define BTN_Y      1
#define BTN_START  9
#define BTN_SELECT 8
#define BTN_L1     4
#define BTN_L2     5
#define BTN_R1     6
#define BTN_R2     7
#define BTN_C1     11
#define BTN_C2     12
#define PIN_JOY2_X A0
#define PIN_JOY2_Y A1

#else
#error "ERROR: MODEL not defined! You probably haven't selected the correct model in config.h"
#endif

// Joystick settings
#define CALIBTIME 10      // Time in seconds to calibrate the joystick
#define DEADZONE 50       // ADC value for 'deadzone' of joystick
#define PULLBACK 100       // Amount to pullback from edge (to make perfect square of movement)

#define INVERT_X1 true   // Invert the X1 axis
#define INVERT_Y1 false   // Invert the Y1 axis
#define INVERT_X2 true   // Invert the X2 axis
#define INVERT_Y2 false   // Invert the Y2 axis

// Auto dim
#define AUTODIM_TIMEOUT 600 * 2 //secs*2 of no input to dim (10mins)

