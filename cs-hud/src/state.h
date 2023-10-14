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
 //----------------------------------------------------------------------------

#ifndef STATE_H
#define STATE_H

//----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "config.h"
#include "defs.h"
// #include "analog_read.h"
#include "batt_mon.h"
#include "shift_in.h"
// #include "gpio.h"
#include "gpio_in.h"
#include "serialport.h"
#include <wiringPi.h>

#include <sys/types.h>
#include <regex.h>

// #include </usr/include/linux/input.h>
#include "keyboard.h"

//----------------------------------------------------------------------------

#define SERIAL_Q_LENGTH 10

struct CS_SERIAL_T {
  char cmd;
  uint8_t data;
};

#define SERIAL_CMD_GET_VOLT 'c'
#define SERIAL_CMD_GET_INFO 'i'
#define SERIAL_CMD_SET_INFO 'I'
#define SERIAL_CMD_GET_VOL  'e'
#define SERIAL_CMD_SET_VOL  'E'
#define SERIAL_CMD_GET_BL   'q'
#define SERIAL_CMD_SET_BL   'Q'
#define SERIAL_CMD_GET_WIFI 'w'
#define SERIAL_CMD_SET_WIFI 'W'
#define SERIAL_CMD_GET_MUTE 'a'
#define SERIAL_CMD_SET_MUTE 'A'
#define SERIAL_CMD_GET_BTN_NOW    'b'
#define SERIAL_CMD_GET_BTN_LAST   'B'
#define SERIAL_CMD_RESET_BTN_LAST 'r'
#define SERIAL_CMD_GET_STATUS     's'
#define SERIAL_CMD_GET_MODEL      'm'
#define SERIAL_CMD_CALIB_JOYSTICK 'J'
#define SERIAL_CMD_SET_LCD        'L'
#define SERIAL_CMD_SET_LCD_INIT   'l'
#define SERIAL_CMD_SET_LCD_CMD    'L'
#define SERIAL_CMD_SET_JOY1_X_INVERT '('
#define SERIAL_CMD_SET_JOY1_Y_INVERT ')'
#define SERIAL_CMD_SET_JOY2_X_INVERT '['
#define SERIAL_CMD_SET_JOY2_Y_INVERT ']'
#define SERIAL_CMD_SET_AVOL       'C'

//----------------------------------------------------------------------------

#define STATE_NONE  0
#define STATE_MODE  1
#define STATE_OSK   2
#define STATE_MENU  3
#define STATE_DEBUG 4

//----------------------------------------------------------------------------

#define BATT_VOLTSCALE 203.5
#define BATT_CURRSCALE 640.0
#define BATT_RESDIVMUL 4.0
#define BATT_RESDIVVAL 1000.0
#define BATT_DACRES    33.0
#define BATT_DACMAX    1023.0

//----------------------------------------------------------------------------

#define WIFI_ICON_COUNT 5
#define WIFI_ERROR   0
#define WIFI_WARNING 1
#define WIFI_SIG_1   2
#define WIFI_SIG_2   3
#define WIFI_SIG_3   4

#define WIFI_SIG_1_LVL 5
#define WIFI_SIG_2_LVL 40
#define WIFI_SIG_3_LVL 55

//----------------------------------------------------------------------------

// struct CS_KEY_T {
  // char     *name;
  // int      value;
  // uint32_t mask;
  // bool     pressed;
// };

struct CS_KEYMAP_T {
  int  value;
  bool pressed;
  bool last;
};

struct CS_KEYSS_T {
  struct CS_KEYMAP_T up;
  struct CS_KEYMAP_T down;
  struct CS_KEYMAP_T left;
  struct CS_KEYMAP_T right;
  struct CS_KEYMAP_T a;
  struct CS_KEYMAP_T b;
  struct CS_KEYMAP_T x;
  struct CS_KEYMAP_T y;
  struct CS_KEYMAP_T start;
  struct CS_KEYMAP_T select;
  struct CS_KEYMAP_T l1;
  struct CS_KEYMAP_T l2;
  struct CS_KEYMAP_T r1;
  struct CS_KEYMAP_T r2;
  struct CS_KEYMAP_T c1;
  struct CS_KEYMAP_T c2;
  struct CS_KEYMAP_T jup;
  struct CS_KEYMAP_T jdown;
  struct CS_KEYMAP_T jleft;
  struct CS_KEYMAP_T jright;
};

struct CS_KEYS_T {
  bool up;
  bool down;
  bool left;
  bool right;
  bool a;
  bool b;
  bool x;
  bool y;
  bool start;
  bool select;
  bool l1;
  bool l2;
  bool r1;
  bool r2;
  bool c1;
  bool c2;
  bool jup;
  bool jdown;
  bool jleft;
  bool jright;
};

struct CS_STATE_T {
  uint8_t state;

  bool is_debug; // NEEDED?
  bool is_osk;   // NEEDED?
  bool is_menu;  // NEEDED?

  bool power_switch_on; //Power switch in ON position?
  bool mode_button_on;  // Mode button pressed?

  bool chrg_state;      // Charging LED on? (currently charging)
  bool pg_state;        // PG LED on? (plugged in to USB)
  bool ext_state;       // USB EXT enabled

  bool wifi_state;      // Is wifi on?
  uint8_t wifi_signal;  // Signal strength in %
  uint8_t wifi_status;  // Visible status of wifi

  bool mute_state;      // Is in mute?
  bool debug_state;
  bool avol_state;
  bool dpad_btns_state;

  bool shutdown_state;

  bool alt_btns_state;
  bool external_switch_state;

  bool joy_button_state; // Is in joystick dpad mode?

  double batt_voltage; // Voltage in V
  double batt_current; // Current in mA

  double batt_voltage_offset; // Offset in V
  double batt_voltage_min;    // Minimum voltage before shutdown
  double batt_voltage_low;    // Minimum voltage before warning (low)
  double batt_voltage_max;    // Maximum voltage for calcs

  int8_t brightness;   // Backlight in %

  int8_t volume;       // Volume in %

  bool   temperature_over;
  double temperature;           // Temperature in *C
  double temperature_threshold; // Temperature fan kicks in
  double temperature_holdoff;   // Reduction in temperature before turning off fan

  struct CS_KEYS_T is_key; // Is a button pressed (e.g. is_key.up)
  struct CS_KEYSS_T gamepad; // Gamepad button array
};

//----------------------------------------------------------------------------

extern volatile struct CS_STATE_T cs_state;

bool state_init(void);
void state_unload(void);
void state_do_poweroff(void);

void state_request_keys(void);

void state_process_aux_gpio(void);
void state_process_slow_serial(void);
void state_process_fast_serial(void);
void state_process_serial(void);
void state_process_system(void);
void state_process_state(void);
void state_process_keys(void);
void state_process_very_slow(void);

//----------------------------------------------------------------------------

#endif
