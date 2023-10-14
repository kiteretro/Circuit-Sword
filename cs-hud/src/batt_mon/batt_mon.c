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

#include "batt_mon.h"

//#define DEBUG_PRINT_TIMING

//-----------------------------------------------------------------------------
// PRIVATE VARIABLES

struct timeval t, tnow;
int data_pos = -1;
uint32_t data_byte = 0;
uint16_t checksum_byte = 0;
#if defined DEBUG_PRINT_TIMING
uint32_t tmp_raw[20];
#endif

volatile uint16_t data_valid = 0;
volatile bool new_data = 0;

//-----------------------------------------------------------------------------
// METHODS

// Calculate 8bit CRC
uint8_t calc_crc8(uint8_t bytes[], uint8_t len) {
	uint16_t crc = 0xFFFF; // initial value
	// loop, calculating CRC for each byte of the string
  uint8_t byteIndex;
	for (byteIndex = 0; byteIndex < len; ++byteIndex) {
		uint8_t bit = 0x80; // initialize bit currently being tested
    uint8_t bitIndex;
		for (bitIndex = 0; bitIndex < 8; ++bitIndex) {
			bool xorFlag = ((crc & 0x8000) == 0x8000);
			crc <<= 1;
			if (((bytes[byteIndex] & bit) ^ (uint8_t)0xff) != (uint8_t)0xff) {
				crc = crc + 1;
			}
			if (xorFlag) {
				crc = crc ^ 0x1021;
			}
			bit >>= 1;
		}
	}
	return (uint8_t)crc;
}

// Callback when interrupt triggered
void batt_mon_callback()
{
  // Time now
  gettimeofday(&tnow, NULL);

  // Calc us elapsed
  uint32_t elapsed = (((tnow.tv_sec - t.tv_sec) * 1000000) + (tnow.tv_usec - t.tv_usec));

  // Set timer
  t = tnow;

  // Build up value data
  if (elapsed > DATA_PULSE_SYNC) {

    data_pos = 0;
    data_byte = 0;
    checksum_byte = 0;

  } else if (data_pos >= 0) {

    if (data_pos < DATA_WIDTH) {
#if defined DEBUG_PRINT_TIMING
      tmp_raw[data_pos] = elapsed;
#endif

      // DATA
      if (elapsed < DATA_PULSE_THRESHOLD) {
        data_byte |= (1 << data_pos);
      }
      data_pos += 1;

    } else if (data_pos < DATA_WIDTH + CHECKSUM_WIDTH) {

      // CHECKSUM
      if (elapsed < DATA_PULSE_THRESHOLD) {
        checksum_byte |= (1 << (data_pos - CHECKSUM_OFFSET));
      }
      data_pos += 1;

      // Final bit?
      if (data_pos == DATA_WIDTH + CHECKSUM_WIDTH) {
        uint8_t tmp_crc = calc_crc8((uint8_t[]){(data_byte >> 8) & 0xFF, data_byte & 0xFF}, 2);
        if (checksum_byte == tmp_crc) {
          data_valid = data_byte;
          new_data = 1;
        } else {
          printf("[!] Checksum failed! DATA:[%x] CHECKSUM:[%x] CRC:[%x]\n", data_byte, checksum_byte, tmp_crc);
#if defined DEBUG_PRINT_TIMING
          uint8_t x;
          for (x=0; x<16; x++) {
            printf("[%zu]", tmp_raw[x]);
          }
          printf("\n");
#endif
        }
      }

    } else {
      printf("[!] Too many batt_mon_callback events\n");
    }
  }
}

void batt_mon_callback_8x3()
{
  // Time now
  gettimeofday(&tnow, NULL);

  // Calc us elapsed
  uint32_t elapsed = (((tnow.tv_sec - t.tv_sec) * 1000000) + (tnow.tv_usec - t.tv_usec));

  // Set timer
  t = tnow;

  // Build up value data
  if (elapsed > DATA_PULSE_SYNC) {

    data_pos = 0;
    data_byte = 0;
    checksum_byte = 0;
    // printf("R\n");

  } else if (data_pos >= 0) {

    if (data_pos < 24) {
#if defined DEBUG_PRINT_TIMING
      tmp_raw[data_pos] = elapsed;
#endif

      // DATA
      if (elapsed < DATA_PULSE_THRESHOLD) {
        data_byte |= (1 << data_pos);
      }
      data_pos += 1;

      // Final bit?
      if (data_pos == 24) {

        uint8_t tmp_data_a = (uint8_t)((data_byte >> 0) & 0xFF);
        uint8_t tmp_data_b = (uint8_t)((data_byte >> 8) & 0xFF);
        uint8_t tmp_data_c = (uint8_t)((data_byte >> 16) & 0xFF);

        if (tmp_data_a == tmp_data_b && tmp_data_a == tmp_data_c) {
          data_valid = tmp_data_a;
          new_data = 1;
          // printf("OK\n");
        } else {
          printf("[!] Checksum failed! [%x]\n", data_byte);
#if defined DEBUG_PRINT_TIMING
          uint8_t x;
          for (x=0; x<16; x++) {
            printf("[%zu]", tmp_raw[x]);
          }
          printf("\n");
#endif

        }

      }

    } else {
      printf("[!] Too many batt_mon_callback events\n");
    }
  }
}

void batt_mon_callback_16x2()
{
  // Time now
  gettimeofday(&tnow, NULL);

  // Calc us elapsed
  uint32_t elapsed = (((tnow.tv_sec - t.tv_sec) * 1000000) + (tnow.tv_usec - t.tv_usec));

  // Set timer
  t = tnow;

  // Build up value data
  if (elapsed > DATA_PULSE_SYNC) {

    data_pos = 0;
    data_byte = 0;
    checksum_byte = 0;
    // printf("R\n");

  } else if (data_pos >= 0) {

    if (data_pos < 32) {
#if defined DEBUG_PRINT_TIMING
      tmp_raw[data_pos] = elapsed;
#endif

      // DATA
      if (elapsed < DATA_PULSE_THRESHOLD) {
        data_byte |= (1 << data_pos);
      }
      data_pos += 1;

      // Final bit?
      if (data_pos == 32) {

        uint16_t tmp_data_a = (uint16_t)((data_byte >> 0) & 0xFFFF);
        uint16_t tmp_data_b = (uint16_t)((data_byte >> 16) & 0xFFFF);

        if (tmp_data_a == tmp_data_b) {
          if (tmp_data_a >= DATA_VALID_MIN && tmp_data_a <= DATA_VALID_MAX) {
            data_valid = tmp_data_a;
            new_data = 1;
            // printf("OK %x\n", data_valid);
          } else {
            printf("[!] Data out of range! [%x]\n", data_byte);
          }
        } else {
          printf("[!] Checksum failed! [%x]\n", data_byte);
#if defined DEBUG_PRINT_TIMING
          uint8_t x;
          for (x=0; x<16; x++) {
            printf("[%zu]", tmp_raw[x]);
          }
          printf("\n");
#endif

        }

      }

    } else {
      printf("[!] Too many batt_mon_callback events\n");
    }
  }
}


// Get voltage
uint16_t batt_mon_voltage()
{
  // printf("[d] RAW: %d\n", data_valid);
  if (data_valid > 0) {
    return (uint16_t)((uint32_t)((uint32_t)1100 * (uint32_t)1023) / data_valid);
  } else {
    return 0;
  }
}

// New data check
bool batt_mon_new_data()
{
  if (new_data == 1) {
    new_data = 0;
    return 1;
  } else {
    return 0;
  }
}

//-----------------------------------------------------------------------------

// Init GPIOs for reading
void batt_mon_init(int pin_data)
{
  printf("[*] batt_mon_init..\n");

  // Set pin modes
  pinMode(pin_data, INPUT);
  pullUpDnControl(pin_data, PUD_UP);

  // Start timer
  gettimeofday(&t, NULL);

  // Register callback
  // wiringPiISR(pin_data, INT_EDGE_FALLING, &batt_mon_callback);
  // wiringPiISR(pin_data, INT_EDGE_FALLING, &batt_mon_callback_8x3);
  wiringPiISR(pin_data, INT_EDGE_FALLING, &batt_mon_callback_16x2);
}

void batt_mon_unload()
{
  printf("[*] batt_mon_unload (TODO!)..\n");
}

//-----------------------------------------------------------------------------
