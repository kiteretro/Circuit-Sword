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

#include "manager.h"

//-----------------------------------------------------------------------------
// PRIVATE VARIABLES
static uint8_t tick = 1;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// METHODS

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void manager_process()
{
  // KEYBOARD
  if (tick % c.interval_keys == 0) {
    state_process_keys();
  }

  // Process EVERY tick:
  state_process_serial();
  state_process_state();

  // GPIO
  if (tick % c.interval_gpio == 0) {
    state_process_aux_gpio();
    state_process_slow_serial();
    state_process_system();

    // Check for shutdown
    if (cs_state.shutdown_state == 1) {
      display_manager_clear();
      state_do_poweroff();
      exit(0);
    }
  }

  // // ANALOG
  // if (tick % INTERVAL_ANALOG == 0) {
  //   // UNUSED SO FAR
  // }

  // SERIAL FAST
  if (tick % c.interval_serial_fast == 0) {
    state_process_fast_serial();
  }

  // // SERIAL
  // if (tick % INTERVAL_SERIAL == 0) {
  //   state_process_serial();
  // }

  // DISPLAY
  if (tick % c.interval_display == 0) {
    display_manager_process();
  }

  // Increment tick
  tick++;
  if (tick > c.interval_max) {
    tick = 1;

    // Very slow operations after x repeats of tick counter
    static uint8_t vslow_counter = 0;
    if (vslow_counter == 2) {
      state_process_very_slow();
      vslow_counter = 0;
    } else {
      vslow_counter++;
    }

#ifdef DEBUG
    printf("[d] power: %i\n", cs_state.power_switch_on);
    printf("[d] chrg:  %i\n", cs_state.chrg_state);
    printf("[d] pg:    %i\n", cs_state.pg_state);
    printf("[d] volt:  %.2f\n", cs_state.batt_voltage);
    printf("[d] bl:    %i\n", cs_state.brightness);
    printf("[d] vol:   %i\n", cs_state.volume);
    printf("[d] mode:  %i\n", cs_state.mode_button_on);
    printf("[d] wifi:  %i\n", cs_state.wifi_state);
    printf("[d] mute:  %i\n", cs_state.mute_state);
    printf("[d] debug: %i\n", cs_state.debug_state);

    printf("[d] U:%i D:%i L:%i R:%i\n", cs_state.is_key.up, cs_state.is_key.down, cs_state.is_key.left, cs_state.is_key.right);
    printf("-------------\n");
#endif
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool manager_init()
{
  printf("[*] manager_init..\n");

  keyboard_init(); // Configure the KEYBOARD feature
  state_init(); // Configure cs state
  display_manager_init(); // Init the display

  return 0;
}

//-----------------------------------------------------------------------------

void manager_unload()
{
  printf("[*] manager_unload..\n");

  state_unload();
  keyboard_unload();
  display_manager_unload();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
