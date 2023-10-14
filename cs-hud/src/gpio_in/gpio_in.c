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

#include "gpio_in.h"

//-----------------------------------------------------------------------------
// PRIVATE VARIABLES

volatile uint32_t allstate = 0;
int allgpios[16];

//-----------------------------------------------------------------------------
// METHODS

// Callback
void gpio_callback_0()  { allstate = (allstate & ~(1 << 0))  | (!digitalRead(allgpios[0])  << 0);  }
void gpio_callback_1()  { allstate = (allstate & ~(1 << 1))  | (!digitalRead(allgpios[1])  << 1);  }
void gpio_callback_2()  { allstate = (allstate & ~(1 << 2))  | (!digitalRead(allgpios[2])  << 2);  }
void gpio_callback_3()  { allstate = (allstate & ~(1 << 3))  | (!digitalRead(allgpios[3])  << 3);  }
void gpio_callback_4()  { allstate = (allstate & ~(1 << 4))  | (!digitalRead(allgpios[4])  << 4);  }
void gpio_callback_5()  { allstate = (allstate & ~(1 << 5))  | (!digitalRead(allgpios[5])  << 5);  }
void gpio_callback_6()  { allstate = (allstate & ~(1 << 6))  | (!digitalRead(allgpios[6])  << 6);  }
void gpio_callback_7()  { allstate = (allstate & ~(1 << 7))  | (!digitalRead(allgpios[7])  << 7);  }
void gpio_callback_8()  { allstate = (allstate & ~(1 << 8))  | (!digitalRead(allgpios[8])  << 8);  }
void gpio_callback_9()  { allstate = (allstate & ~(1 << 9))  | (!digitalRead(allgpios[9])  << 9);  }
void gpio_callback_10() { allstate = (allstate & ~(1 << 10)) | (!digitalRead(allgpios[10]) << 10); }
void gpio_callback_11() { allstate = (allstate & ~(1 << 11)) | (!digitalRead(allgpios[11]) << 11); }
void gpio_callback_12() { allstate = (allstate & ~(1 << 12)) | (!digitalRead(allgpios[12]) << 12); }
void gpio_callback_13() { allstate = (allstate & ~(1 << 13)) | (!digitalRead(allgpios[13]) << 13); }
void gpio_callback_14() { allstate = (allstate & ~(1 << 14)) | (!digitalRead(allgpios[14]) << 14); }
void gpio_callback_15() { allstate = (allstate & ~(1 << 15)) | (!digitalRead(allgpios[15]) << 15); }

// Get state
uint32_t gpio_in_state()
{
  return allstate;
}

//-----------------------------------------------------------------------------

// Init GPIOs for reading
void gpio_in_init(int *data, uint8_t count)
{
  printf("[i] gpio_in_init..\n");

  uint8_t i;
  for (i=0; i<count; i++) {

    // Remember the pin
    allgpios[i] = data[i];

    // Inint if not -1
    if (allgpios[i] > -1) {
      printf("[i] gpio_in_init pin %d as callback %d..\n", data[i], i);

      // Pull up input
      pinMode(allgpios[i], INPUT);
      pullUpDnControl(allgpios[i], PUD_UP);

      // Register callback
      if (i == 0) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_0);
      } else if (i == 1) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_1);
      } else if (i == 2) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_2);
      } else if (i == 3) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_3);
      } else if (i == 4) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_4);
      } else if (i == 5) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_5);
      } else if (i == 6) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_6);
      } else if (i == 7) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_7);
      } else if (i == 8) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_8);
      } else if (i == 9) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_9);
      } else if (i == 10) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_10);
      } else if (i == 11) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_11);
      } else if (i == 12) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_12);
      } else if (i == 13) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_13);
      } else if (i == 14) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_14);
      } else if (i == 15) {
        wiringPiISR(allgpios[i], INT_EDGE_BOTH, &gpio_callback_15);
      } else {
        printf("[!] UNKNOWN %d\n", i);
      }
    }
  }
}

void gpio_in_unload()
{
  printf("[*] gpio_in_unload (TODO!)..\n");
}

//-----------------------------------------------------------------------------
