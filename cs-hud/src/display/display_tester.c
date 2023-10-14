/*
 * giles test display
 */

#include <stdio.h>
#include "display.h"

//-----------------------------------------------------------------------------


// MAIN
int main(void)
{
  printf("Raspberry Pi display\n");

  display_init();

  usleep(1000000);

  display_unload();

  return 0;
}
