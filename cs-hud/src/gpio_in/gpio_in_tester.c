/*
 * giles test gpio in
 */

#include <stdio.h>
#include "gpio_in.h"

#define PIN_DATA  12

// MAIN
int main(void)
{
  printf("Raspberry Pi gpio in\n");

  if (wiringPiSetupGpio() == -1)
    return 1;
  
  int data[8] = { 12, 1, 16, 20, 26, 7, 6, 5 };
  gpio_in_init(data, 8);
  
  for (;;) {

    uint32_t res = gpio_in_state();
    if (res == 0) {
      printf("0x00\n");
    } else {
      printf("%#04x\n", res);
    }

    delay(16);

  }
  
  gpio_in_unload();

  return 0;
}
