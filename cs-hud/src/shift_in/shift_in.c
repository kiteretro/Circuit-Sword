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

#include "shift_in.h"

//-----------------------------------------------------------------------------
// PRIVATE VARIABLES

int pin_data  = 22;
int pin_latch = 27;
int pin_clock = 23;

//-----------------------------------------------------------------------------
// METHODS

// Perform the shift in (array)
void shift_in_data_array(uint8_t *data, uint8_t count)
{
  // Set default pin states
  digitalWrite(pin_clock, 0);
  digitalWrite(pin_latch, 0);
  digitalWrite(pin_latch, 1);

  uint8_t byte_pos = 0;
  uint8_t bit_pos = 0;

  // For each byte
  for (byte_pos = 0; byte_pos < count; byte_pos++) {
    uint8_t data_byte = 0;

    // For each bit
    for (bit_pos = 0; bit_pos < 8; bit_pos++) {
      data_byte |= digitalRead(pin_data) << bit_pos;

      digitalWrite(pin_clock, 1);
      digitalWrite(pin_clock, 0);
    }

    // Set the value in return array
    data[byte_pos] = data_byte;
  }

}

//-----------------------------------------------------------------------------

// Perform the shift in (32 bit)
uint32_t shift_in_data_32(uint8_t count)
{
  // Set default pin states
  digitalWrite(pin_clock, 0);
  digitalWrite(pin_latch, 0);
  digitalWrite(pin_latch, 1);

  uint8_t byte_pos = 0;
  uint8_t bit_pos = 0;
  uint8_t pos = 0;
  uint32_t result = 0;

  // For each byte
  for (byte_pos = 0; byte_pos < count; byte_pos++) {

    // For each bit
    for (bit_pos = 0; bit_pos < 8; bit_pos++) {
      result |= digitalRead(pin_data) << pos;
      pos++;

      digitalWrite(pin_clock, 1);
      digitalWrite(pin_clock, 0);
    }
  }

  return result;
}

//-----------------------------------------------------------------------------

// Init GPIOs for shiting
void shift_in_init(int data, int latch, int clk)
{
  // Set vars
  pin_data = data;
  pin_latch = latch;
  pin_clock = clk;

  // Set pin modes
  pinMode(pin_data, INPUT);
  pinMode(pin_latch, OUTPUT);
  pinMode(pin_clock, OUTPUT);
}

//-----------------------------------------------------------------------------
