/*
 * giles test analog in
 */

#include <stdio.h>
#include "analog_read.h"

#define PIN_VOLTAGE 26

// MAIN
int main(void)
{
  printf("Raspberry Pi analog in\n");

  if (wiringPiSetupGpio() == -1)
    return 1;

  read_analog_init(PIN_VOLTAGE);
  read_analog_start();

  for (;;) {

    if (read_analog_process()) {
      read_analog_start();
    }

    delay(500);

  }

  return 0;
}
