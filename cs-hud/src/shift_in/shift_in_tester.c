/*
 * giles test shift in
 */

#include <stdio.h>
#include "shift_in.h"

#define PIN_DATA  22
#define PIN_LATCH 27
#define PIN_CLOCK 23

#define DATA_LENGTH 3
uint8_t data[DATA_LENGTH];

// MAIN
int main(void)
{
  printf("Raspberry Pi shift in\n");

  if (wiringPiSetupGpio() == -1)
    return 1;

  shift_in_init(PIN_DATA, PIN_LATCH, PIN_CLOCK);

  for (;;) {

    // shift_in_data_array(data, DATA_LENGTH);
    //
    // uint8_t pos = 0;
    // for (pos = 0; pos < DATA_LENGTH; pos++) {
    //   printf("%#02x ", data[pos]);
    // }
    // printf("\n");

    uint32_t res = shift_in_data_32(DATA_LENGTH);
    printf("%#06x\n", res);

    delay(16);

  }

  return 0;
}
