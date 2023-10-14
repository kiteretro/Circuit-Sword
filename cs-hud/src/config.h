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

#ifndef CONFIG_H
#define CONFIG_H

//----------------------------------------------------------------------------

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "defs.h"

#include <sys/types.h>
#include <regex.h>

//----------------------------------------------------------------------------

#define MODEL_CIRCUIT_SWORD      1
#define MODEL_CIRCUIT_SWORD_LITE 2
#define MODEL_CIRCUIT_GEM        3

#define MODEL MODEL_CIRCUIT_SWORD
// #define MODEL MODEL_CIRCUIT_SWORD_LITE
// #define MODEL MODEL_CIRCUIT_GEM
// #define DEBUG // In make file

//----------------------------------------------------------------------------

#define INPUT_NONE 0
#define INPUT_GPIO 1
#define INPUT_SHIFT 2
#define INPUT_GAMEPAD 3
#define INPUT_SERIAL 4
#define INPUT_DUMB_WIRE 5

#define DISPLAY_BASIC 1
#define DISPLAY_ADVANCED 2

#define DISPLAY_TYPE_ON_PWRSW 1
#define DISPLAY_TYPE_ON_MODE 2

#define DISABLED 0
#define ENABLED 1

//----------------------------------------------------------------------------

struct CS_CONFIG_T {

  // SETTINGS
  uint8_t model;
  uint8_t setting_input;
  uint8_t setting_display;
  uint8_t setting_display_type;
  uint8_t setting_pwrsw;
  uint8_t setting_mode;
  uint8_t setting_pg;
  uint8_t setting_chrg;
  uint8_t setting_ext;
  uint8_t setting_batt;
  uint8_t setting_bl;
  uint8_t setting_vol;
  uint8_t setting_serial;
  uint8_t setting_shutdown;
  uint8_t setting_vtx;
  bool    setting_pwrsw_menu;
  bool    setting_read_rfkill_state;

  // SHIFT IN
  int shift_in_pin_data;
  int shift_in_pin_latch;
  int shift_in_pin_clock;
  uint8_t shift_in_data_len;
  uint8_t shift_in_key_len;

  uint32_t shift_in_mask_buttons;
  uint32_t shift_in_mask_auxs;

  uint32_t shift_in_mask_mode;
  uint32_t shift_in_mask_ext;
  uint32_t shift_in_mask_pg;
  uint32_t shift_in_mask_chrg;

  uint32_t shift_in_mask_up;
  uint32_t shift_in_mask_down;
  uint32_t shift_in_mask_left;
  uint32_t shift_in_mask_right;
  uint32_t shift_in_mask_start;
  uint32_t shift_in_mask_select;
  uint32_t shift_in_mask_a;
  uint32_t shift_in_mask_b;
  uint32_t shift_in_mask_x;
  uint32_t shift_in_mask_y;
  uint32_t shift_in_mask_c1;
  uint32_t shift_in_mask_c2;
  uint32_t shift_in_mask_l1;
  uint32_t shift_in_mask_l2;
  uint32_t shift_in_mask_r1;
  uint32_t shift_in_mask_r2;
  uint32_t shift_in_mask_j_up;
  uint32_t shift_in_mask_j_down;
  uint32_t shift_in_mask_j_left;
  uint32_t shift_in_mask_j_right;

  // GPIO IN
  int gpio_in_up;
  int gpio_in_down;
  int gpio_in_left;
  int gpio_in_right;
  int gpio_in_start;
  int gpio_in_select;
  int gpio_in_a;
  int gpio_in_b;
  int gpio_in_x;
  int gpio_in_y;
  int gpio_in_c1;
  int gpio_in_c2;
  int gpio_in_l1;
  int gpio_in_l2;
  int gpio_in_r1;
  int gpio_in_r2;

  // BATT MON
  int batt_mon_pin_data;

  // GPIO
  int gpio_pin_pwrsw;
  int gpio_pin_mode;
  int gpio_pin_pg;
  int gpio_pin_chrg;
  int gpio_pin_wifi;
  int gpio_pin_overtemp;
  int gpio_pin_auxsw;
  int gpio_pin_vtx;

  // INTERVAL
  uint8_t interval_max;
  uint8_t interval_gpio;
  uint8_t interval_keys;
  uint8_t interval_serial_fast;
  uint8_t interval_display;

  // MODES
  bool hide_battery;
  double hide_battery_val;

  // SERIAL
  const char* serial_port;
};

//----------------------------------------------------------------------------

extern volatile struct CS_CONFIG_T c;

bool config_init(void);
void config_unload(void);

//----------------------------------------------------------------------------

#endif
