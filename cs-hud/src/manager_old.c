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

volatile double data_voltage = 0;

volatile uint32_t data_raw_input = 0;

volatile bool data_is_mode = 0;
volatile bool data_is_plugged = 0;
volatile bool data_is_charging = 0;
volatile bool data_is_ext = 0;
volatile bool data_is_poweroff = 0;

//-----------------------------------------------------------------------------

KB_KEY_T kb_keys_map[NUMBER_OF_KEYS] =
{
  { "UP",      KEY_UP,    MAP_UP     , 0 },
  { "DOWN",    KEY_DOWN,  MAP_DOWN   , 0 },
  { "LEFT",    KEY_LEFT,  MAP_LEFT   , 0 },
  { "RIGHT",   KEY_RIGHT, MAP_RIGHT  , 0 },
  { "START",   KEY_ENTER, MAP_START  , 0 },
  { "SELECT",  KEY_ESC,   MAP_SELECT , 0 },
  { "A",       KEY_Z,     MAP_A      , 0 },
  { "B",       KEY_X,     MAP_B      , 0 },
  { "X",       KEY_A,     MAP_X      , 0 },
  { "Y",       KEY_S,     MAP_Y      , 0 },
  { "C1",      KEY_Q,     MAP_C1     , 0 },
  { "C2",      KEY_W,     MAP_C2     , 0 },
  { "L1",      KEY_1,     MAP_L1     , 0 },
  { "L2",      KEY_2,     MAP_L2     , 0 },
  { "R1",      KEY_3,     MAP_R1     , 0 },
  { "R2",      KEY_4,     MAP_R2     , 0 },
  { "J_UP",    KEY_I,     MAP_J_UP   , 0 },
  { "J_DOWN",  KEY_K,     MAP_J_DOWN , 0 },
  { "J_LEFT",  KEY_J,     MAP_J_LEFT , 0 },
  { "J_RIGHT", KEY_L,     MAP_J_RIGHT, 0 }
};

uint8_t tick = 1;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// METHODS

bool manager_is_button_pressed(uint32_t mask)
{
  return !(data_raw_input & mask);
}

//-----------------------------------------------------------------------------

void manager_process_analog()
{
  // Process ANALOG reads
#ifdef USE_VBATT_GPIO
  if (read_analog_process()) {

    // Collect latest average
    data_voltage = read_analog_voltage_average();

    // Start a new read
    read_analog_start();
  }
#endif
}

//-----------------------------------------------------------------------------

void manager_process_gpio()
{
  // Process GPIO power switch
  data_is_poweroff = !gpio_read_power_pin();
}

//-----------------------------------------------------------------------------

void manager_process_shift_in()
{
  // Process the SHIFT registers
  data_raw_input = shift_in_data_32(DATA_LENGTH);
  // printf("[d] %#06x\n", data_raw_input);
}

//-----------------------------------------------------------------------------

void manager_process_keyboard()
{
  // Process actions to go to the keyboard
  // Check each key that is registered
  uint8_t k;
  for (k = 0; k < NUMBER_OF_KEYS ; k++) {
    // Check all buttons

    // if (!(data_raw_input & kb_keys_map[k].mask)) {
    if (manager_is_button_pressed(kb_keys_map[k].mask)) {
      // Key is pressed

      if (!kb_keys_map[k].pressed) {
        printf("[i] %s PRESSED\n", kb_keys_map[k].name);
        keyboard_press(kb_keys_map[k].value);
        kb_keys_map[k].pressed = 1;
      }

    } else if (kb_keys_map[k].pressed) {
      // Release the key

      keyboard_release(kb_keys_map[k].value);
      kb_keys_map[k].pressed = 0;

    }

  }
}

//-----------------------------------------------------------------------------

void manager_process_aux()
{
  // Process the AUX commands
  // Mode is a toggle
  if (manager_is_button_pressed(MAP_MODE)) {
    if (!data_is_mode) {
      data_is_mode = 1;
    }
  } else {
    if (data_is_mode) {
      data_is_mode = 0;
    }
  }

  // Assign power states
  data_is_plugged = manager_is_button_pressed(MAP_PGOOD);
  data_is_charging = manager_is_button_pressed(MAP_CHRG);
  data_is_ext = manager_is_button_pressed(MAP_EXT);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void manager_process()
{
  // KEYBOARD
  if (tick % INTERVAL_KEYBOARD == 0) {
    // Read and set states
#ifdef USE_INPUT_SHIFT
    manager_process_shift_in();

    // Process states
    manager_process_aux(); // TODO FIX THIS METHOD FOR GENERIC

    // Switch based on mode (TMP!)
    if (data_is_mode) {

      // Voltage calib
      if (manager_is_button_pressed(MAP_UP)) {
        config_val_r += 0.1;
        printf("config_val_r=%f\n", config_val_r);
      }
      if (manager_is_button_pressed(MAP_DOWN)) {
        config_val_r -= 0.1;
        printf("config_val_r=%f\n", config_val_r);
      }

    } else {

      // Process keyboard writing
      manager_process_keyboard(); //TODO NEEDS TO BE MORE GENERIC

    }
#endif
  }

  // GPIO
  if (tick % INTERVAL_GPIO == 0) {
    manager_process_gpio();
  }

  // ANALOG
  if (tick % INTERVAL_ANALOG == 0) {

    // Read from ADC
#ifdef USE_VBATT_GPIO
    manager_process_analog();
#endif

    // Update screen
    display_manager_draw_battery(data_voltage);
  }

  // DISPLAY ??????
  display_manager_process();

  // Increment tick
  tick++;
  if (tick > INTERVAL_MAX) {
    tick = 1;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool manager_init()
{
  printf("[*] manager_init..\n");

  printf("[i] state: %i\n", cs_state.temp);

  // Start GPIO library
  if (wiringPiSetupGpio() == -1) {
    return 1;
  }

  // Configure the GPIO power switch
  gpio_set_power_pin(PIN_POWER);
  gpio_init();

#ifdef USE_USB_SERIAL
  serial_init(SERIAL_DEVICE);
#endif

  // Configure the ANANLOG read pin
#ifdef USE_VBATT_GPIO
  read_analog_init(PIN_VOLTAGE);
  read_analog_start();
#endif

  // Configure the SHIFT IN feature
#ifdef USE_INPUT_SHIFT
  shift_in_init(PIN_DATA, PIN_LATCH, PIN_CLOCK);
#endif

  // Configure the KEYBOARD feature
  keyboard_init();

  // Init the display
  display_manager_init();

  return 0;
}

//-----------------------------------------------------------------------------

void manager_unload()
{
  printf("[*] manager_unload..\n");

  // Keyboard
  keyboard_unload();

  // Serial
#ifdef USE_USB_SERIAL
  serial_unload();
#endif

  // Display
  display_manager_unload();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
