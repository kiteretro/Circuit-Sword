//-----------------------------------------------------------------------------
/*
 * The GPL v3 License
 *
 * Kite's Circuit Sword
 * Copyright (C) 2017 Giles Burgess (Kite's Item Shop)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//-----------------------------------------------------------------------------

#ifndef DEFS_H
#define DEFS_H

//-----------------------------------------------------------------------------

// #define MODEL_CIRCUIT_SWORD      1
// #define MODEL_CIRCUIT_SWORD_LITE 2
// #define MODEL_CIRCUIT_GEM        3

// //-----------------------------------------------------------------------------

// #define MODEL MODEL_CIRCUIT_SWORD
// // #define DEBUG // In make file

//-----------------------------------------------------------------------------
// These should really be in a config file (loadable)..

//#define USE_USB_SERIAL

//#define USE_BL_PWM
//#define USE_BL_USB_SERIAL

//#define USE_VOL_GPIO
//#define USE_VOL_USB_SERIAL

//#define USE_VBATT_GPIO
//#define USE_VBATT_USB_SERIAL
//#define USE_VBATT_ONEWIRE

//#define USE_INPUT_SHIFT
//#define USE_INPUT_GPIO
//#define USE_INPUT_USB_SERIAL

//#define USE_MENU_SWITCH
//#define USE_MENU_USB_SERIAL
//#define USE_MENU_HOLD_BTN
//#define USE_MENU_GPIO

//-----------------------------------------------------------------------------

// #if MODEL == MODEL_CIRCUIT_SWORD

// #define USE_USB_SERIAL
// #define USE_BL_USB_SERIAL
// #define USE_VOL_USB_SERIAL
// #define USE_VBATT_USB_SERIAL
// #define USE_INPUT_USB_SERIAL
// #define USE_MENU_USB_SERIAL

// #else

// #error "ERROR: THIS MODEL NOT SUPPORTED (YET)"

// #endif

//-----------------------------------------------------------------------------
// Loop timer values % 60

#define INTERVAL_MAX 60
// #define INTERVAL_ANALOG 60
#define INTERVAL_GPIO 60
// #define INTERVAL_SERIAL 5
#define INTERVAL_SERIAL_FAST 10
// #define INTERVAL_KEYBOARD 1
#define INTERVAL_DISPLAY 10

//-----------------------------------------------------------------------------
// Generic GPIO

#define PIN_POWER    37
#define PIN_CHRG     36
#define PIN_PG       38
#define PIN_OVERTEMP 35
#define PIN_WIFI_EN  34

//-----------------------------------------------------------------------------
// USE_USB_SERIAL

#define SERIAL_DEVICE "/dev/ttyACM0"

//-----------------------------------------------------------------------------
// USE_INPUT_SHIFT

#define PIN_DATA  22
#define PIN_LATCH 27
#define PIN_CLOCK 23
#define DATA_LENGTH 3

#define NUMBER_OF_KEYS 20

#define BUTTON_MASK 0b111100001111111111111111
#define AUX_MASK    0b000011110000000000000000

#define MAP_MODE    0b000010000000000000000000
#define MAP_EXT     0b000001000000000000000000
#define MAP_PGOOD   0b000000100000000000000000
#define MAP_CHRG    0b000000010000000000000000

#define MAP_UP      0b000000000000000000000001
#define MAP_DOWN    0b000000000000000000000100
#define MAP_LEFT    0b000000000000000000000010
#define MAP_RIGHT   0b000000000000000000001000
#define MAP_START   0b000000000000000001000000
#define MAP_SELECT  0b000000000000000010000000
#define MAP_A       0b000000000100000000000000
#define MAP_B       0b000000000010000000000000
#define MAP_X       0b000000001000000000000000
#define MAP_Y       0b000000000001000000000000
#define MAP_C1      0b000000000000000000100000
#define MAP_C2      0b000000000000000000010000
#define MAP_L1      0b000000000000001000000000
#define MAP_L2      0b000000000000010000000000
#define MAP_R1      0b000000000000000100000000
#define MAP_R2      0b000000000000100000000000
#define MAP_J_UP    0b100000000000000000000000
#define MAP_J_DOWN  0b010000000000000000000000
#define MAP_J_LEFT  0b001000000000000000000000
#define MAP_J_RIGHT 0b000100000000000000000000

//-----------------------------------------------------------------------------
// USE_VBATT_GPIO

#define PIN_VOLTAGE 26

//-----------------------------------------------------------------------------

#endif
