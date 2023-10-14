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

#ifndef ANALOG_READ_H
#define ANALOG_READ_H

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <wiringPi.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#include <math.h>

//-----------------------------------------------------------------------------
// STATIC VARIABLES

#define AVERAGE_SIZE 5

//-----------------------------------------------------------------------------
// EXTERNAL VARIABLES

extern float config_val_vc;
extern float config_val_r;
extern float config_val_c;

//-----------------------------------------------------------------------------
// METHODS

double read_analog_voltage_average(void);
void read_analog_start(void);
bool read_analog_process(void);
void read_analog_init(int pin);

//-----------------------------------------------------------------------------

#endif
