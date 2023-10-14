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

#ifndef GPIO_IN_H
#define GPIO_IN_H

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <wiringPi.h>
#include <stdbool.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
// STATIC VARIABLES


//-----------------------------------------------------------------------------
// METHODS

void gpio_in_init(int *data, uint8_t count);
uint32_t gpio_in_state(void);
void gpio_in_unload(void);

//-----------------------------------------------------------------------------

#endif
