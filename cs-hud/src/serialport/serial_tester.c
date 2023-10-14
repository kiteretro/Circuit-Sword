/*
 * giles test gpio in
 */

#include <stdio.h>
#include "serialport.h"

#define SERIAL_DEVICE "/dev/ttyACM0"

// MAIN
int main(void)
{
  printf("Raspberry Pi serial\n");

  serial_init(SERIAL_DEVICE);

  serial_send("V", 1);
  usleep(1000000);

  char rx_buffer[256];
  uint8_t ret = serial_receive(rx_buffer, 255);
  fprintf(stdout, "Got %i bytes read : %s\n", ret, rx_buffer);

  serial_unload();

  return 0;
}
