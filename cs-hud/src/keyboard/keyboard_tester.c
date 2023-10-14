/*
 * giles test shift in gamepad
 */

 /*
  * Add udev rule (will overwrite if present)
	* echo "SUBSYSTEM==\"input\", ATTRS{name}==\"retrogame\", ENV{ID_INPUT_KEYBOARD}=\"1\"" > /etc/udev/rules.d/10-retrogame.rules
  */

#include <stdio.h>
#include "keyboard.h"

//-----------------------------------------------------------------------------

KB_CS_KEY_T kb_keys_map[1] =
{
  { "ENTER", KEY_ENTER, 0b000000000000000000000000, 0 },
};

//-----------------------------------------------------------------------------

// MAIN
int main(void)
{
  printf("Raspberry Pi keyboard\n");

  keyboard_init();

  keyboard_button(kb_keys_map[0].value);

  keyboard_unload();

  return 0;
}
