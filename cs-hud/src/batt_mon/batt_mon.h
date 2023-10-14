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

#ifndef BATT_MON_H
#define BATT_MON_H

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <wiringPi.h>
#include <stdbool.h>
#include <stdint.h>

#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

//-----------------------------------------------------------------------------
// STATIC VARIABLES

// #define DATA_PULSE_SYNC 7000
#define DATA_PULSE_SYNC 25000
#define DATA_PULSE_THRESHOLD 9500
#define DATA_WIDTH 16
#define CHECKSUM_WIDTH 8
#define CHECKSUM_OFFSET DATA_WIDTH
#define DATA_VALID_MIN 210
#define DATA_VALID_MAX 400

//-----------------------------------------------------------------------------
// METHODS

void batt_mon_init(int data);
uint8_t calc_crc8(uint8_t bytes[], uint8_t len);
uint16_t batt_mon_voltage(void);
bool batt_mon_new_data(void);
void batt_mon_unload(void);

//-----------------------------------------------------------------------------

#endif
