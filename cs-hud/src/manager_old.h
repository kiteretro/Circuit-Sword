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

#ifndef MANAGER_H
#define MANAGER_H

//-----------------------------------------------------------------------------

#include <stdio.h>

#include "defs.h"
#include "analog_read.h"
#include "shift_in.h"
#include "keyboard.h"
#include "gpio.h"
#include "display_manager.h"
#include "serialport.h"
#include "state.h"

//-----------------------------------------------------------------------------

struct KB_KEY_T {
  char *name;
  int   value;
  uint32_t mask;
  bool pressed;
};

//-----------------------------------------------------------------------------

extern volatile double data_voltage;
extern volatile uint32_t data_raw_input;
extern volatile bool data_is_mode;
extern volatile bool data_is_plugged;
extern volatile bool data_is_charging;
extern volatile bool data_is_ext;
extern volatile bool data_is_poweroff;

//-----------------------------------------------------------------------------

bool manager_init();
void manager_unload();
void manager_process();

//-----------------------------------------------------------------------------

#endif
