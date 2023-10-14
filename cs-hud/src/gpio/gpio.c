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

#include "gpio.h"

//-----------------------------------------------------------------------------
// PRIVATE VARIABLES

int pin_power  = 24;

//-----------------------------------------------------------------------------
// METHODS

// Perform a read of power switch pin
bool gpio_read_power_pin()
{
  // Return the read result
  return digitalRead(pin_power);
}

//-----------------------------------------------------------------------------

// Init all gpios congfigured
void gpio_init()
{
  // Set pin modes
  pinMode(pin_power, INPUT);
}

//-----------------------------------------------------------------------------

// Set the power switch pin to read
void gpio_set_power_pin(int pin)
{
  // Set the pin for reading
  pin_power = pin;
}

//-----------------------------------------------------------------------------
