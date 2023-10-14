/*
 * giles test gpio in
 */

#include <stdio.h>
#include "gpio.h"

#define PIN_POWER 4

// MAIN
int main(void)
{
  printf("Raspberry Pi gpio\n");

  if (wiringPiSetupGpio() == -1)
    return 1;

  gpio_set_power_pin(PIN_POWER);
  gpio_init();

  for (;;) {

    printf("Power pin: %d\n", gpio_read_power_pin());

    delay(500);

  }

  return 0;
}
