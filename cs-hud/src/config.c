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

#include "config.h"

//-----------------------------------------------------------------------------

// Default some sensible values
volatile struct CS_CONFIG_T c = {

  // SETTINGS
  .model                = MODEL,
  .setting_input        = INPUT_NONE,
  .setting_display      = INPUT_NONE,
  .setting_display_type = INPUT_NONE,
  .setting_pwrsw        = INPUT_NONE,
  .setting_mode         = INPUT_NONE,
  .setting_pg           = INPUT_NONE,
  .setting_chrg         = INPUT_NONE,
  .setting_ext          = INPUT_NONE,
  .setting_batt         = INPUT_NONE,
  .setting_bl           = DISABLED,
  .setting_vol          = DISABLED,
  .setting_serial       = DISABLED,
  .setting_shutdown     = DISABLED,
  .setting_vtx          = DISABLED,
  .setting_pwrsw_menu   = DISABLED,
  .setting_read_rfkill_state = DISABLED, // Read the output from 'rfkill' to set what the wifi state SHOULD be (on or off)

  // SHIFT IN
  .shift_in_pin_data   = -1,
  .shift_in_pin_latch  = -1,
  .shift_in_pin_clock  = -1,
  .shift_in_data_len   = -1,
  .shift_in_key_len    = -1,

  .shift_in_mask_buttons = 0b111100001111111111111111,
  .shift_in_mask_auxs    = 0b000011110000000000000000,

  .shift_in_mask_mode    = 0b000010000000000000000000,
  .shift_in_mask_ext     = 0b000001000000000000000000,
  .shift_in_mask_pg      = 0b000000100000000000000000,
  .shift_in_mask_chrg    = 0b000000010000000000000000,
  // .shift_in_mask_mode    = 19,
  // .shift_in_mask_ext     = 18,
  // .shift_in_mask_pg      = 17,
  // .shift_in_mask_chrg    = 16,

  .shift_in_mask_up      = 0b000000000000000000000001,
  .shift_in_mask_down    = 0b000000000000000000000100,
  .shift_in_mask_left    = 0b000000000000000000000010,
  .shift_in_mask_right   = 0b000000000000000000001000,
  .shift_in_mask_start   = 0b000000000000000001000000,
  .shift_in_mask_select  = 0b000000000000000010000000,
  .shift_in_mask_a       = 0b000000000100000000000000,
  .shift_in_mask_b       = 0b000000000010000000000000,
  .shift_in_mask_x       = 0b000000001000000000000000,
  .shift_in_mask_y       = 0b000000000001000000000000,
  .shift_in_mask_c1      = 0b000000000000000000100000,
  .shift_in_mask_c2      = 0b000000000000000000010000,
  .shift_in_mask_l1      = 0b000000000000001000000000,
  .shift_in_mask_l2      = 0b000000000000010000000000,
  .shift_in_mask_r1      = 0b000000000000000100000000,
  .shift_in_mask_r2      = 0b000000000000100000000000,
  .shift_in_mask_j_up    = 0b100000000000000000000000,
  .shift_in_mask_j_down  = 0b010000000000000000000000,
  .shift_in_mask_j_left  = 0b001000000000000000000000,
  .shift_in_mask_j_right = 0b000100000000000000000000,
  // .shift_in_mask_up      = 0,
  // .shift_in_mask_down    = 2,
  // .shift_in_mask_left    = 1,
  // .shift_in_mask_right   = 3,
  // .shift_in_mask_start   = 6,
  // .shift_in_mask_select  = 7,
  // .shift_in_mask_a       = 14,
  // .shift_in_mask_b       = 13,
  // .shift_in_mask_x       = 15,
  // .shift_in_mask_y       = 12,
  // .shift_in_mask_c1      = 5,
  // .shift_in_mask_c2      = 4,
  // .shift_in_mask_l1      = 9,
  // .shift_in_mask_l2      = 10,
  // .shift_in_mask_r1      = 8,
  // .shift_in_mask_r2      = 11,
  // .shift_in_mask_j_up    = 23,
  // .shift_in_mask_j_down  = 22,
  // .shift_in_mask_j_left  = 21,
  // .shift_in_mask_j_right = 20,

  // GPIO IN
  .gpio_in_up     = -1,
  .gpio_in_down   = -1,
  .gpio_in_left   = -1,
  .gpio_in_right  = -1,
  .gpio_in_start  = -1,
  .gpio_in_select = -1,
  .gpio_in_a      = -1,
  .gpio_in_b      = -1,
  .gpio_in_x      = -1,
  .gpio_in_y      = -1,
  .gpio_in_c1     = -1,
  .gpio_in_c2     = -1,
  .gpio_in_l1     = -1,
  .gpio_in_l2     = -1,
  .gpio_in_r1     = -1,
  .gpio_in_r2     = -1,

  // BATT MON
  .batt_mon_pin_data = -1,

  // GPIO
  .gpio_pin_pwrsw    = -1,
  .gpio_pin_mode     = -1,
  .gpio_pin_pg       = -1,
  .gpio_pin_chrg     = -1,
  .gpio_pin_wifi     = -1,
  .gpio_pin_overtemp = -1,
  .gpio_pin_auxsw    = -1,
  .gpio_pin_vtx      = -1,

  // INTERVAL
  .interval_max         = 60,
  .interval_gpio        = 60,
  .interval_keys        = 1,
  .interval_serial_fast = 10,
  .interval_display     = 10,

  // SERIAL
  .serial_port = "/dev/ttyACM0",

  // MODES
  .hide_battery = 0,
  .hide_battery_val = 3.60,

};

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

bool config_init()
{
  printf("[*] config_init..\n");

#if MODEL == MODEL_CIRCUIT_SWORD

  c.setting_input        = INPUT_SERIAL;
  c.setting_mode         = INPUT_SERIAL;
  c.setting_batt         = INPUT_SERIAL;
  c.setting_vol          = ENABLED;
  c.setting_serial       = ENABLED;
  c.setting_shutdown     = ENABLED;

  c.gpio_pin_pwrsw    = 37;
  c.gpio_pin_pg       = 38;
  c.gpio_pin_chrg     = 36;
  c.gpio_pin_wifi     = 34;
  c.gpio_pin_overtemp = 35;

#elif MODEL == MODEL_CIRCUIT_SWORD_LITE

  c.setting_batt  = INPUT_DUMB_WIRE;
  c.setting_input = INPUT_SHIFT;
  c.setting_mode  = INPUT_SHIFT;
  c.setting_pg    = INPUT_SHIFT;
  c.setting_chrg  = INPUT_SHIFT;
  c.setting_ext   = INPUT_SHIFT;
  c.setting_shutdown = ENABLED;
  c.setting_read_rfkill_state = ENABLED;

  c.batt_mon_pin_data = 26;
  c.gpio_pin_pwrsw    = 24;

  c.shift_in_pin_data   = 22;
  c.shift_in_pin_latch  = 27;
  c.shift_in_pin_clock  = 23;
  c.shift_in_data_len   = 3;
  c.shift_in_key_len    = 20;

#elif MODEL == MODEL_CIRCUIT_GEM

  c.setting_batt  = INPUT_DUMB_WIRE;
  c.setting_input = INPUT_GPIO;
  c.setting_pwrsw_menu = ENABLED;
  c.setting_read_rfkill_state = ENABLED;
  c.setting_vol = ENABLED;
  // c.setting_vtx = ENABLED;

  c.batt_mon_pin_data = 23;
  c.gpio_pin_pwrsw    = 4;
  // c.gpio_pin_mode     = 6;
  c.gpio_pin_vtx = 27;

  c.gpio_in_up     = 12;
  c.gpio_in_down   = 1;
  c.gpio_in_left   = 16;
  c.gpio_in_right  = 20;
  c.gpio_in_start  = 5;
  c.gpio_in_select = 6;
  c.gpio_in_a      = 26;
  c.gpio_in_b      = 7;
  c.gpio_in_l1     = 0;
  c.gpio_in_r1     = 22;
  c.gpio_in_c1     = 2;
  c.gpio_in_c2     = 3;
  c.gpio_in_l2     = 14;
  c.gpio_in_r2     = 15;

#else
#error "ERROR: THIS MODEL NOT SUPPORTED (YET)"
#endif

  return 0;
}

//-----------------------------------------------------------------------------

void config_unload()
{
  printf("[*] config_unload..\n");
}
