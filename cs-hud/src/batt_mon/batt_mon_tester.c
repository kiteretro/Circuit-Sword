/*
 * giles test batt mon
 */

#include <stdio.h>
#include "batt_mon.h"
#include <stdlib.h>

#define PIN_VBAT  23

#define DATA_LENGTH 3
uint8_t data[DATA_LENGTH];

// MAIN
int main(void)
{
  printf("Raspberry Pi batt mon\n");

  if (wiringPiSetupGpio() == -1)
    return 1;

  // uint8_t tmp_crc = calc_crc8((uint8_t[]){ 130, 0 }, 2);
  // printf("CRC: %x\n", tmp_crc);
  //
  // uint8_t i;
  // for (i=8; i>0; i--) {
  //   if ((tmp_crc >> (i)) & 1) {
  //     printf("1");
  //   } else {
  //     printf("0");
  //   }
  // }
  // printf("\n");
  //
  // exit(0);

  batt_mon_init(PIN_VBAT);

  for (;;) {

    if (batt_mon_new_data() == 1) {
      printf("%d\n", batt_mon_voltage());
    }
    delay(100);

  }

  return 0;
}
