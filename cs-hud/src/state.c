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

#include "state.h"

//-----------------------------------------------------------------------------

// Default some sensible values
volatile struct CS_STATE_T cs_state = {
  .state            = STATE_NONE,
  .power_switch_on  = true,
  .mode_button_on   = false,

  .wifi_state       = true,
  .wifi_signal      = 0,
  .wifi_status      = WIFI_ERROR,

  .batt_voltage     = 0.01,
  .batt_voltage_min = 3.20,
  .batt_voltage_low = 3.25,
  .batt_voltage_max = 4.00,

  .brightness  = -1,
  .volume      = -1,

  .temperature_over = true,
  .temperature = 0.0,
  .temperature_threshold = 60.0,
  .temperature_holdoff = 55.0,
  // .temperature_threshold = 40.0,
  // .temperature_holdoff = 35.0,
};

uint8_t qPos = 0;
uint8_t qCount = 0;

struct CS_SERIAL_T q[SERIAL_Q_LENGTH];

//-----------------------------------------------------------------------------

uint16_t convertFrom8To16(uint8_t dataFirst, uint8_t dataSecond)
{
  uint16_t dataBoth = 0x0000;

  dataBoth = dataFirst;
  dataBoth = dataBoth << 8;
  dataBoth |= dataSecond;
  return dataBoth;
}

//-----------------------------------------------------------------------------

void add_to_serial_queue(uint8_t cmd, uint8_t data)
{
  if (c.setting_serial == ENABLED) {
    if (qCount < SERIAL_Q_LENGTH - 1) {
      // Determine new pos
      uint8_t new_qPos = qPos + qCount;
      if (new_qPos >= SERIAL_Q_LENGTH) {
        new_qPos -= SERIAL_Q_LENGTH;
      }

      // Create the packet
      struct CS_SERIAL_T p = {
        .cmd = cmd,
        .data = data,
      };

      // Add to queue
      q[new_qPos] = p;
      qCount++;
      // printf("[i] ADDED [%i][%i] at [%i]\n", p.cmd, p.data, new_qPos);
    } else {
      printf("[#] WARNING: Serial queue is full, cannot add [%i][%i] to it\n", cmd, data);
    }
  }
}

//-----------------------------------------------------------------------------

int8_t get_volume()
{
  int strength = 20;
  int i;
  FILE *fd;

  // Open wifi file
  fd = popen("amixer sget PCM", "r");
  if (fd == NULL) {
    printf("[!] ERROR: Failed to read amixer volume\n");
    return -1;
  }

  int len = 32;
  char buf[len];

  // skip first five lines
  for (i = 0; i < 5; i++) {
    fgets(buf, len, fd);
  }

  char s_string[] = {'['};
  uint16_t s_pos = 0;
  uint16_t s_hit = sizeof(s_string)/sizeof(s_string[0]);

  bool searching = true;
  while (searching) {
    // For each line of output
    if (fgets(buf, len, fd)) {

      bool correct_line = false;
      uint8_t num_len = 0;
      uint8_t num_count = 0;
      char result[10];

      for (i = 0; i < len; i++) {
        // printf("[%c]", buf[i]);

        if (correct_line) {

          // If is a number
          if (buf[i] >= '0' && buf[i] <= '9') {

            // Add to output
            result[num_len] = buf[i];
            num_len++;

          // else if percentage
          } else if (buf[i] == '%') {

              num_count++;

              // Add term char
              result[num_len] = '\0';

              // convert output to number
              sscanf(result, "%d", &strength);

              // end the search
              searching = false;
              i = len;

          } else {
            // reset number
            num_len = 0;
          }

        } else {

          if (buf[i] == s_string[s_pos]) {
            s_pos++;
          } else {
            // Reset search
            if (s_pos > 0) {
              s_pos = 0;
            }
          }
          // This line is right with wlan0?
          if (s_pos == s_hit) {
            correct_line = true;
          }
        }
      }
    } else {
      searching = false;
    }
  }

  // We're done with the file
  pclose(fd);

  if (strength > 100) {
    return 100;
  } else if (strength < 0) {
    return 0;
  } else {
    return (strength & 0xFF);
  }
}

bool get_wifi()
{
  bool status = 0;
  int i;
  FILE *fd;

  // Open wifi file
  fd = popen("/usr/sbin/rfkill list wlan", "r");
  if (fd == NULL) {
    printf("[!] ERROR: Failed to read rfkill\n");
    return -1;
  }

  int len = 32;
  char buf[len];

  char s_string[] = {'S', 'o', 'f', 't', ' ', 'b', 'l', 'o', 'c', 'k', 'e', 'd', ':', ' ', 'n', 'o'};
  uint16_t s_pos = 0;
  uint16_t s_hit = sizeof(s_string)/sizeof(s_string[0]);

  bool searching = true;
  while (searching) {
    // For each line of output
    if (fgets(buf, len, fd)) {

      bool correct_line = false;

      for (i = 0; i < len; i++) {
        // printf("[%c]", buf[i]);

        if (correct_line) {

          status = 1;

        } else {

          if (buf[i] == s_string[s_pos]) {
            s_pos++;
          } else {
            // Reset search
            if (s_pos > 0) {
              s_pos = 0;
            }
          }
          // This line is right search
          if (s_pos == s_hit) {
            correct_line = true;
          }
        }
      }
    } else {
      searching = false;
    }
  }

  // We're done with the file
  pclose(fd);

  return status;
}

void state_do_poweroff()
{
  FILE *fd;

  // Perform safe action first
  if (access( "/opt/cs_shutdown.sh", X_OK ) != -1) {
    fd = popen("sudo /opt/cs_shutdown.sh", "r");
    if (fd == NULL) {
      printf("[!] ERROR: Failed to run /opt/cs_shutdown.sh before power down\n");
    } else {
      printf("[*] Running /opt/cs_shutdown.sh..\n");

      char buf[256];
      while (fgets(buf, sizeof(buf)-1, fd) != NULL) {
        printf("[d] %s", buf);
      }

      pclose(fd);
      exit(0);
    }
    pclose(fd);

  } else {
    printf("[!] ERROR: /opt/cs_shutdown.sh does not exist or is not executable. Unable to run.\n");
  }

  usleep(250000); //250ms

  // Now close down the system
  fd = popen("sudo /sbin/shutdown -h now", "r");
  if (fd == NULL) {
    printf("[!] ERROR: Failed to power off\n");
  } else {
    printf("[*] Powering off system..\n");
  }
  pclose(fd);

  exit(0);
}

// TODO NOT FINISHED
uint32_t get_filevar(const char* path)
{
  uint32_t val = 0;
  FILE *fd;

  // Open file
  fd = fopen(path, "r");
  if (fd == NULL) {
    printf("[!] ERROR: Failed to read file\n");
    return 0;
  }

  fclose(fd);

  return val;
}

// TODO NOT FINISHED
void set_filevar(const char* path, uint32_t val)
{
  // fd = popen("touch ", "r");
  // if (fd == NULL) {
  //   printf("[!] ERROR: Failed to open file to set\n");
  //   return;
  // }
  // pclose(fd);

  FILE *f = fopen(path, "w");
  if (f == NULL) {
    printf("[!] ERROR: Failed to open file!\n");
    return;
  }

  /* print some text */
  // const char *text = "Write this to the file";
  // fprintf(f, "Some text: %s\n", text);
  fprintf(f, ":%d\n", val);

  // /* print integers and floats */
  // int i = 1;
  // float py = 3.1415927;
  // fprintf(f, "Integer: %d, float: %f\n", i, py);
  //
  // /* printing single chatacters */
  // char c = 'A';
  // fprintf(f, "A character: %c\n", c);

  fclose(f);
}

//-----------------------------------------------------------------------------

bool state_init()
{
  printf("[*] state_init..\n");

  // Start GPIO library
  if (wiringPiSetupGpio() == -1) {
    return 1;
  }

  // GPIOs
  if (c.gpio_pin_overtemp > -1) {
    pinMode(c.gpio_pin_overtemp, OUTPUT);
    digitalWrite(c.gpio_pin_overtemp, 0);
  }

  if (c.gpio_pin_wifi > -1) {
    pinMode(c.gpio_pin_wifi, OUTPUT);
    digitalWrite(c.gpio_pin_wifi, 1);
  }

  // SERIAL
  if (c.setting_serial == ENABLED) {
    serial_init(SERIAL_DEVICE);
    serial_clear();
  }

  // INPUT METHOD
  if (c.setting_input == INPUT_SHIFT) {
    shift_in_init(c.shift_in_pin_data, c.shift_in_pin_latch, c.shift_in_pin_clock);
  } else if (c.setting_input == INPUT_GPIO) {

    int gpio_in_pins_counter = 0;
    int gpio_in_pins[16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_up     > -1 ? c.gpio_in_up     : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_down   > -1 ? c.gpio_in_down   : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_left   > -1 ? c.gpio_in_left   : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_right  > -1 ? c.gpio_in_right  : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_start  > -1 ? c.gpio_in_start  : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_select > -1 ? c.gpio_in_select : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_a      > -1 ? c.gpio_in_a      : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_b      > -1 ? c.gpio_in_b      : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_x      > -1 ? c.gpio_in_x      : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_y      > -1 ? c.gpio_in_y      : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_c1     > -1 ? c.gpio_in_c1     : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_c2     > -1 ? c.gpio_in_c2     : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_l1     > -1 ? c.gpio_in_l1     : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_l2     > -1 ? c.gpio_in_l2     : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_r1     > -1 ? c.gpio_in_r1     : -1;
    gpio_in_pins[gpio_in_pins_counter++] =  c.gpio_in_r2     > -1 ? c.gpio_in_r2     : -1;

    gpio_in_init(gpio_in_pins, 16);
  }

  // BATT READ
  if (c.setting_batt == INPUT_DUMB_WIRE) {
    batt_mon_init(c.batt_mon_pin_data);
  }

  // VOLUME
  if (c.setting_vol == ENABLED) {
    cs_state.volume = get_volume();
    printf("[i] Found system volume already at %d\n", cs_state.volume);
  }

  // WIFI initial state
  if (c.setting_read_rfkill_state == ENABLED) {
    cs_state.wifi_state = get_wifi();
    printf("[i] Found system wifi state already at %d\n", cs_state.wifi_state);
  }

  // CONF
  cs_state.gamepad.up.value     = KEY_UP;
  cs_state.gamepad.down.value   = KEY_DOWN;
  cs_state.gamepad.left.value   = KEY_LEFT;
  cs_state.gamepad.right.value  = KEY_RIGHT;
  cs_state.gamepad.start.value  = KEY_ENTER;
  cs_state.gamepad.select.value = KEY_ESC;
  cs_state.gamepad.a.value      = KEY_Z;
  cs_state.gamepad.b.value      = KEY_X;
  cs_state.gamepad.x.value      = KEY_A;
  cs_state.gamepad.y.value      = KEY_S;
  cs_state.gamepad.c1.value     = KEY_Q;
  cs_state.gamepad.c2.value     = KEY_W;
  cs_state.gamepad.l1.value     = KEY_1;
  cs_state.gamepad.l2.value     = KEY_2;
  cs_state.gamepad.r1.value     = KEY_3;
  cs_state.gamepad.r2.value     = KEY_4;
  cs_state.gamepad.jup.value    = KEY_I;
  cs_state.gamepad.jdown.value  = KEY_K;
  cs_state.gamepad.jleft.value  = KEY_J;
  cs_state.gamepad.jright.value = KEY_L;

  return 0;
}

//-----------------------------------------------------------------------------

void process_temperature()
{
  double temp;

  // Read temperature
  FILE *fd;
  fd = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
  if (fd == NULL) {
    printf("[!] ERROR: Failed to read CPU temperature\n");
    return;
  }

  fscanf(fd, "%lf", &temp);
  fclose(fd);

  if (temp > 0) {
    temp /= 1000;
  }
  // printf("[d] The temperature is %f C.\n", temp);

  if (temp > 0 && temp < 160) {
    cs_state.temperature = temp;
  } else {
    printf("[!] ERROR: CPU temperature not a good number: %f\n", temp);
  }

  // Fan enable
  if (cs_state.temperature > cs_state.temperature_threshold) {
    if (!cs_state.temperature_over) {
      if (c.gpio_pin_overtemp > -1) {
        printf("[d] FAN ON\n");
        digitalWrite(c.gpio_pin_overtemp, 0);
      }
      cs_state.temperature_over = true;
    }
  } else {
    if (cs_state.temperature_over) {
      if (cs_state.temperature < cs_state.temperature_holdoff) {
        if (c.gpio_pin_overtemp > -1) {
          printf("[d] FAN OFF\n");
          digitalWrite(c.gpio_pin_overtemp, 1);
        }
        cs_state.temperature_over = false;
      }
    }
  }
}

void process_volume()
{
  // Actions:
  //  System volume level

  static int volume_last = -1;

  if (cs_state.volume != volume_last) {
    if (cs_state.volume >= 0 && cs_state.volume <= 100) {

      // Build command string
      char cmd[32];
      snprintf(cmd, sizeof(cmd), "amixer sset PCM %i%%", cs_state.volume);

      // Apply the volume
      FILE *fd;
      fd = popen(cmd, "r");
      if (fd == NULL) {
        printf("[!] ERROR: Failed to set volume with amixer\n");
      } else {
        printf("[*] Setting volume to [%i]..\n", cs_state.volume);
        // usleep(1000); //1ms
      }
      pclose(fd);

      volume_last = cs_state.volume;
    }
  }
}

void process_wifi()
{
  // Sets:
  //  wifi_signal (in %)
  //  wifi_status (in STATE)
  // Actions:
  //  Wifi block/unblock using rfkill

  int strength = 0;
  bool adapter_found = false;
  int i;
  FILE *fd;

  // Open wifi file
  fd = fopen("/proc/net/wireless", "r");
  if (fd == NULL) {
    printf("[!] ERROR: Failed to read wifi file\n");
    return;
  }

  int len = 32;
  char buf[len];

  // skip first two lines
  for (i = 0; i < 2; i++) {
    fgets(buf, len, fd);
  }

  char s_string[] = {'w', 'l', 'a', 'n', '0', ':'};
  uint16_t s_pos = 0;
  uint16_t s_hit = sizeof(s_string)/sizeof(s_string[0]);

  bool searching = true;
  while (searching) {
    // For each line of output
    if (fgets(buf, len, fd)) {

      bool correct_line = false;
      uint8_t num_len = 0;
      uint8_t num_count = 0;
      char result[10];

      for (i = 0; i < len; i++) {
        // printf("[%c]", buf[i]);

        if (correct_line) {

          // If is a number
          if (buf[i] >= '0' && buf[i] <= '9') {

            // Add to output
            result[num_len] = buf[i];
            num_len++;

          // else if dot
          } else if (buf[i] == '.') {

              num_count++;

              // Is this the 2nd one?
              if (num_count == 2) {

                // Add term char
                result[num_len] = '\0';

                // convert output to number
                sscanf(result, "%d", &strength);

                // end the search
                searching = false;
                i = len;

              } else {
                // reset number
                num_len = 0;
              }

          } else {
            // reset number
            num_len = 0;
          }

        } else {

          if (buf[i] == s_string[s_pos]) {
            s_pos++;
          } else {
            // Reset search
            if (s_pos > 0) {
              s_pos = 0;
            }
          }
          // This line is right with wlan0?
          if (s_pos == s_hit) {
            correct_line = true;
            adapter_found = true;
          }
        }
      }
    } else {
      searching = false;
    }
  }

  // We're done with the file
  fclose(fd);

  // Finally, set some values
  if (strength > 100) {
    strength = 100;
  } else if (strength < 0) {
    strength = 0;
  }

  cs_state.wifi_signal = (uint8_t)strength;

  static bool last_wifi_state = true;
  if (cs_state.wifi_state) {
    // Is meant to be on

    if (!last_wifi_state) {
      // Turn it on
      fd = popen("/usr/sbin/rfkill unblock all", "r");
      if (fd == NULL) {
        printf("[!] ERROR: Failed to turn on wifi\n");
      } else {
        printf("[*] Wifi turning on..\n");
        if (c.gpio_pin_wifi > -1) {
          digitalWrite(c.gpio_pin_wifi, 1);
          // usleep(20000); //20ms
        }
      }
      pclose(fd);
      last_wifi_state = true;
    }

    if (adapter_found) {
      // Adapter is online
      if (cs_state.wifi_signal > WIFI_SIG_3_LVL) {
        cs_state.wifi_status = WIFI_SIG_3;
      } else if (cs_state.wifi_signal > WIFI_SIG_2_LVL) {
        cs_state.wifi_status = WIFI_SIG_2;
      } else if (cs_state.wifi_signal > WIFI_SIG_1_LVL) {
        cs_state.wifi_status = WIFI_SIG_1;
      } else {
        cs_state.wifi_status = WIFI_WARNING;
      }
    } else {
      // No adapter!
      cs_state.wifi_status = WIFI_ERROR;
    }
  } else {
    // Is meant to be off
    cs_state.wifi_status = WIFI_WARNING;

    if (last_wifi_state) {
      // Turn it off
      fd = popen("/usr/sbin/rfkill block all", "r");
      if (fd == NULL) {
        printf("[!] ERROR: Failed to turn off wifi\n");
      } else {
        printf("[*] Wifi turning off..\n");
        if (c.gpio_pin_wifi > -1) {
          digitalWrite(c.gpio_pin_wifi, 0);
          // usleep(20000); //20ms
        }
      }
      pclose(fd);
      last_wifi_state = false;
    }
  }

  // printf("[d] WIFI STRENGTH: %i\n", cs_state.wifi_signal);
  // printf("[d] WIFI STATUS  : %i\n", cs_state.wifi_status);
}

//-----------------------------------------------------------------------------

void state_request_keys()
{
  // Request keys in a special case (menu?)
  add_to_serial_queue(SERIAL_CMD_GET_BTN_LAST, 0);
}

//-----------------------------------------------------------------------------

void state_process_very_slow()
{
  // VOLUME update from system
  // TODO: This doesnt work well when used with AVOL
  /*if (c.setting_vol == ENABLED) {
    uint8_t newvol = get_volume();
    if (newvol != cs_state.volume) {
      cs_state.volume = newvol;
      printf("[i] Found new system volume at %d\n", cs_state.volume);
      add_to_serial_queue(SERIAL_CMD_SET_VOL, cs_state.volume);
    }
  }*/

  // WIFI update from system
  if (c.setting_read_rfkill_state == ENABLED) {
    bool newwifi = get_wifi();
    if (newwifi != cs_state.wifi_state) {
      cs_state.wifi_state = newwifi;
      printf("[i] Found new system wifi state at %d\n", cs_state.wifi_state);
      add_to_serial_queue(SERIAL_CMD_SET_WIFI, cs_state.wifi_state);
    }
  }
}

void state_process_aux_gpio()
{
  // Read any configured GPIOs
  if (c.gpio_pin_pwrsw > -1) {
    if (c.setting_pwrsw_menu) {
      cs_state.mode_button_on = !digitalRead(c.gpio_pin_pwrsw);
    } else {
      cs_state.power_switch_on = digitalRead(c.gpio_pin_pwrsw);
    }
  }
  if (c.gpio_pin_chrg > -1) {
    cs_state.chrg_state = digitalRead(c.gpio_pin_chrg);
  }
  if (c.gpio_pin_pg > -1) {
    cs_state.pg_state = digitalRead(c.gpio_pin_pg);
  }
  if (c.gpio_pin_mode > -1) {
    cs_state.mode_button_on = !digitalRead(c.gpio_pin_mode);
  }

  // Process power (good a place as any)
  if (c.setting_shutdown == ENABLED) {
    if (!cs_state.power_switch_on) {
      // Power off
      // do_poweroff();
      cs_state.shutdown_state = 1;
    }
  }
}

void state_process_slow_serial()
{
  if (c.setting_input == INPUT_SERIAL) {
    add_to_serial_queue(SERIAL_CMD_GET_VOLT, 0);
    add_to_serial_queue(SERIAL_CMD_GET_BL, 0);
  }
}

void state_process_fast_serial()
{
  if (c.setting_input == INPUT_SERIAL) {
    add_to_serial_queue(SERIAL_CMD_GET_VOL, 0);
    add_to_serial_queue(SERIAL_CMD_GET_STATUS, 0);

    if (cs_state.state == STATE_OSK || cs_state.state == STATE_MENU) {
      add_to_serial_queue(SERIAL_CMD_GET_BTN_LAST, 0);
    }
  }
}

//-----------------------------------------------------------------------------

void process_key(volatile struct CS_KEYMAP_T *key)
{
  // Deal with a key pointer and process it
  if (!key->pressed) {
    if (key->last) {
      // JUST RELEASED
      keyboard_release(key->value);
      key->last = 0;
    }
  } else {
    if (!key->last) {
      // JUST PRESSED
      keyboard_press(key->value);
      key->last = 1;
    }
  }
}

void state_process_keys()
{
  // Input methods
  if (c.setting_input == INPUT_SHIFT) {
    // Shift in gamepad

    // Read 32bits
    uint32_t res = shift_in_data_32(c.shift_in_data_len);
    // printf("%#06x\n", res);

    // Get keys
    cs_state.gamepad.up.pressed     = !(res & c.shift_in_mask_up);
    cs_state.gamepad.down.pressed   = !(res & c.shift_in_mask_down);
    cs_state.gamepad.left.pressed   = !(res & c.shift_in_mask_left);
    cs_state.gamepad.right.pressed  = !(res & c.shift_in_mask_right);
    cs_state.gamepad.a.pressed      = !(res & c.shift_in_mask_a);
    cs_state.gamepad.b.pressed      = !(res & c.shift_in_mask_b);
    cs_state.gamepad.x.pressed      = !(res & c.shift_in_mask_x);
    cs_state.gamepad.y.pressed      = !(res & c.shift_in_mask_y);
    cs_state.gamepad.start.pressed  = !(res & c.shift_in_mask_start);
    cs_state.gamepad.select.pressed = !(res & c.shift_in_mask_select);
    cs_state.gamepad.c1.pressed     = !(res & c.shift_in_mask_c1);
    cs_state.gamepad.c2.pressed     = !(res & c.shift_in_mask_c2);
    cs_state.gamepad.l1.pressed     = !(res & c.shift_in_mask_l1);
    cs_state.gamepad.r1.pressed     = !(res & c.shift_in_mask_r1);
    cs_state.gamepad.l2.pressed     = !(res & c.shift_in_mask_l2);
    cs_state.gamepad.r2.pressed     = !(res & c.shift_in_mask_r2);
    cs_state.gamepad.jup.pressed    = !(res & c.shift_in_mask_j_up);
    cs_state.gamepad.jdown.pressed  = !(res & c.shift_in_mask_j_down);
    cs_state.gamepad.jleft.pressed  = !(res & c.shift_in_mask_j_left);
    cs_state.gamepad.jright.pressed = !(res & c.shift_in_mask_j_right);

    // Send keys
    if (cs_state.state == STATE_NONE) {

      process_key(&cs_state.gamepad.up);
      process_key(&cs_state.gamepad.down);
      process_key(&cs_state.gamepad.left);
      process_key(&cs_state.gamepad.right);
      process_key(&cs_state.gamepad.a);
      process_key(&cs_state.gamepad.b);
      process_key(&cs_state.gamepad.x);
      process_key(&cs_state.gamepad.y);
      process_key(&cs_state.gamepad.l1);
      process_key(&cs_state.gamepad.r1);
      process_key(&cs_state.gamepad.l2);
      process_key(&cs_state.gamepad.r2);
      process_key(&cs_state.gamepad.start);
      process_key(&cs_state.gamepad.select);
      process_key(&cs_state.gamepad.c1);
      process_key(&cs_state.gamepad.c2);
      process_key(&cs_state.gamepad.jup);
      process_key(&cs_state.gamepad.jdown);
      process_key(&cs_state.gamepad.jleft);
      process_key(&cs_state.gamepad.jright);
    }

    // Process any additional bits if enabled
    if (c.setting_mode == INPUT_SHIFT) { cs_state.mode_button_on = !(res & c.shift_in_mask_mode); }
    if (c.setting_pg == INPUT_SHIFT) { cs_state.pg_state = (res & c.shift_in_mask_pg); }
    if (c.setting_chrg == INPUT_SHIFT) { cs_state.chrg_state = (res & c.shift_in_mask_chrg); }
    if (c.setting_ext == INPUT_SHIFT) { cs_state.ext_state = (res & c.shift_in_mask_ext); }

  } else if (c.setting_input == INPUT_GPIO) {
    // GPIO input

    // Read 32bits
    uint32_t res = gpio_in_state();
    // printf("%#06x\n", res);

    // Get keys
    cs_state.gamepad.up.pressed     = (res & 0b000000000000000000000001);
    cs_state.gamepad.down.pressed   = (res & 0b000000000000000000000010);
    cs_state.gamepad.left.pressed   = (res & 0b000000000000000000000100);
    cs_state.gamepad.right.pressed  = (res & 0b000000000000000000001000);
    cs_state.gamepad.a.pressed      = (res & 0b000000000000000001000000);
    cs_state.gamepad.b.pressed      = (res & 0b000000000000000010000000);
    cs_state.gamepad.x.pressed      = (res & 0b000000000000000100000000);
    cs_state.gamepad.y.pressed      = (res & 0b000000000000001000000000);
    cs_state.gamepad.c1.pressed     = (res & 0b000000000000010000000000);
    cs_state.gamepad.c2.pressed     = (res & 0b000000000000100000000000);
    cs_state.gamepad.l1.pressed     = (res & 0b000000000001000000000000);
    cs_state.gamepad.r1.pressed     = (res & 0b000000000010000000000000);
    cs_state.gamepad.l2.pressed     = (res & 0b000000000100000000000000);
    cs_state.gamepad.r2.pressed     = (res & 0b000000001000000000000000);
    if (cs_state.alt_btns_state) {
      cs_state.gamepad.x.pressed      = (res & 0b000000000000000000010000);
      cs_state.gamepad.y.pressed      = (res & 0b000000000000000000100000);
    } else {
      cs_state.gamepad.start.pressed  = (res & 0b000000000000000000010000);
      cs_state.gamepad.select.pressed = (res & 0b000000000000000000100000);
    }

    // Send keys
    if (cs_state.state == STATE_NONE) {

      process_key(&cs_state.gamepad.up);
      process_key(&cs_state.gamepad.down);
      process_key(&cs_state.gamepad.left);
      process_key(&cs_state.gamepad.right);
      process_key(&cs_state.gamepad.a);
      process_key(&cs_state.gamepad.b);
      process_key(&cs_state.gamepad.x);
      process_key(&cs_state.gamepad.y);
      process_key(&cs_state.gamepad.l1);
      process_key(&cs_state.gamepad.r1);
      process_key(&cs_state.gamepad.l2);
      process_key(&cs_state.gamepad.r2);
      process_key(&cs_state.gamepad.start);
      process_key(&cs_state.gamepad.select);
      process_key(&cs_state.gamepad.c1);
      process_key(&cs_state.gamepad.c2);
    }
  }
}

//-----------------------------------------------------------------------------

void state_process_serial()
{
  // Process anything on the serial queue
  if (qCount > 0) {
    // printf("[d] Processing serial cmd [%i][%i] qPos:%i qCount:%i\n", q[qPos].cmd, q[qPos].data, qPos, qCount);
    char rx_buffer[32];
    char tx_buffer[8];

    // VOLTAGE
    if (q[qPos].cmd == SERIAL_CMD_GET_VOLT) {

      // serial_clear();
      serial_send(&q[qPos].cmd, 1);
      int ret = serial_receive_bytes(rx_buffer, 2, 100);

      if (ret == 2) {
        uint16_t v = convertFrom8To16(rx_buffer[1], rx_buffer[0]);
        if (v > 1023) {
          v = 1023;
        }
        v = (uint16_t)((( (float)v * BATT_VOLTSCALE * BATT_DACRES + ( BATT_DACMAX * 5 ) ) / (( BATT_DACRES * BATT_RESDIVVAL ) / BATT_RESDIVMUL)));
        if (v > 550) {
          v = 550;
        }

        if (v > 0) {
          cs_state.batt_voltage = (double)(v/100.0);
        } else {
          cs_state.batt_voltage = 0.1;
        }

      } else {
        printf("[!] Unexpected bytes returned for %c: %i\n", q[qPos].cmd, ret);
      }

    // BACKLIGHT
    } else if (q[qPos].cmd == SERIAL_CMD_GET_BL) {

      // serial_clear();
      serial_send(&q[qPos].cmd, 1);
      int ret = serial_receive_bytes(rx_buffer, 1, 100);

      if (ret == 1) {
        uint8_t bl = rx_buffer[0];
        if (bl > 100) {
          bl = 100;
        }
        cs_state.brightness = bl;

      } else {
        printf("[!] Unexpected bytes returned for %c: %i\n", q[qPos].cmd, ret);
      }

    // GET VOLUME
    } else if (q[qPos].cmd == SERIAL_CMD_GET_VOL) {

      // serial_clear();
      serial_send(&q[qPos].cmd, 1);
      int ret = serial_receive_bytes(rx_buffer, 1, 100);

      if (ret == 1) {
        uint8_t vol = rx_buffer[0];
        if (vol > 100) {
          vol = 100;
        }
        cs_state.volume = vol;

        // Process the volume RIGHT NOW (instant feedback)
        process_volume();

      } else {
        printf("[!] Unexpected bytes returned for %c: %i\n", q[qPos].cmd, ret);
      }

    // STATUS (b[][][DPAD][AVOL][INFO][AUD][WIFI][MODE])
    } else if (q[qPos].cmd == SERIAL_CMD_GET_STATUS) {

      // serial_clear();
      serial_send(&q[qPos].cmd, 1);
      int ret = serial_receive_bytes(rx_buffer, 1, 100);

      if (ret == 1) {
        uint8_t dat = rx_buffer[0];

        cs_state.mode_button_on = dat & (1 << 0);
        cs_state.wifi_state     = dat & (1 << 1);
        cs_state.mute_state   = !(dat & (1 << 2));
        cs_state.debug_state    = dat & (1 << 3);
        cs_state.avol_state     = dat & (1 << 4);
        cs_state.dpad_btns_state= dat & (1 << 5);

      } else {
        printf("[!] Unexpected bytes returned for %c: %i\n", q[qPos].cmd, ret);
      }

    // BUTTON RESET
    } else if (q[qPos].cmd == SERIAL_CMD_RESET_BTN_LAST) {

      // serial_clear();
      serial_send(&q[qPos].cmd, 1);
      int ret = serial_receive_bytes(rx_buffer, 2, 100);

      if (ret != 2) {
        printf("[!] Unexpected bytes returned for %c: %i\n", q[qPos].cmd, ret);
      }

    // BUTTON LAST
    } else if (q[qPos].cmd == SERIAL_CMD_GET_BTN_LAST) {

      // serial_clear();
      serial_send(&q[qPos].cmd, 1);
      int ret = serial_receive_bytes(rx_buffer, 2, 100);

      if (ret == 2) {
        cs_state.gamepad.up.pressed     = rx_buffer[0] & (1 << 0);
        cs_state.gamepad.down.pressed   = rx_buffer[0] & (1 << 1);
        cs_state.gamepad.left.pressed   = rx_buffer[0] & (1 << 2);
        cs_state.gamepad.right.pressed  = rx_buffer[0] & (1 << 3);
        cs_state.gamepad.a.pressed      = rx_buffer[0] & (1 << 4);
        cs_state.gamepad.b.pressed      = rx_buffer[0] & (1 << 5);
        cs_state.gamepad.x.pressed      = rx_buffer[0] & (1 << 6);
        cs_state.gamepad.y.pressed      = rx_buffer[0] & (1 << 7);

        cs_state.gamepad.start.pressed  = rx_buffer[1] & (1 << 0);
        cs_state.gamepad.select.pressed = rx_buffer[1] & (1 << 1);
        cs_state.gamepad.l1.pressed     = rx_buffer[1] & (1 << 2);
        cs_state.gamepad.l2.pressed     = rx_buffer[1] & (1 << 3);
        cs_state.gamepad.r1.pressed     = rx_buffer[1] & (1 << 4);
        cs_state.gamepad.r2.pressed     = rx_buffer[1] & (1 << 5);
        cs_state.gamepad.c1.pressed     = rx_buffer[1] & (1 << 6);
        cs_state.gamepad.c2.pressed     = rx_buffer[1] & (1 << 7);

      } else {
        printf("[!] Unexpected bytes returned for %c: %i\n", q[qPos].cmd, ret);
      }

    // SET VOLUME
    } else if (q[qPos].cmd == SERIAL_CMD_SET_VOL) {

      // serial_clear();
      tx_buffer[0] = q[qPos].cmd;
      tx_buffer[1] = q[qPos].data;
      serial_send(tx_buffer, 2);
      int ret = serial_receive_bytes(rx_buffer, 2, 100);

      if (ret != 2) {
        printf("[!] Unexpected bytes returned for %c: %i\n", q[qPos].cmd, ret);
      }

    // SET WIFI
    } else if (q[qPos].cmd == SERIAL_CMD_SET_WIFI) {

      // serial_clear();
      tx_buffer[0] = q[qPos].cmd;
      if (q[qPos].data == 0) {
        tx_buffer[1] = '0';
      } else {
        tx_buffer[1] = '1';
      }
      serial_send(tx_buffer, 2);
      int ret = serial_receive_bytes(rx_buffer, 2, 100);

      if (ret != 2) {
        printf("[!] Unexpected bytes returned for %c: %i\n", q[qPos].cmd, ret);
      }

    // UNKNOWN
    } else {
      printf("[!] Unsupported serial command %i\n", q[qPos].cmd);
    }

    qPos++;
    if (qPos == SERIAL_Q_LENGTH) {
      qPos = 0;
    }
    qCount--;

  }
}

void state_process_system()
{
  // Precess system bits
  process_temperature();
  process_volume();
  process_wifi();

  // Process alternative ways of collecting data
  if (c.setting_batt == INPUT_DUMB_WIRE) {
    if (batt_mon_new_data()) {
      cs_state.batt_voltage = (double)batt_mon_voltage() / 1000;
    }
  }
}

void state_process_state()
{
  // Determine what should currently be displayed
  if (cs_state.mode_button_on) {
    cs_state.state = STATE_MODE;

  } else if (cs_state.debug_state) {
    cs_state.state = STATE_OSK;

  } else if (cs_state.state != STATE_MENU) {
    cs_state.state = STATE_NONE;

  } else {
    printf("[!] Invalid display state: %i", cs_state.state);
    cs_state.state = STATE_NONE;
  }
  // printf("[d] STATE = %i\n", cs_state.state);

  // Process any actions while in this state
  // Lite specific
  if (c.model == MODEL_CIRCUIT_SWORD_LITE) {
    if (c.setting_input == INPUT_SHIFT) {
      if (cs_state.state == STATE_MODE) {
        // OSK button pressed
        static bool debug_pressed_last = 0;
        if (cs_state.gamepad.a.pressed) {
          if (!debug_pressed_last) {
            debug_pressed_last = 1;
            cs_state.debug_state = !cs_state.debug_state;
          }
        } else {
          debug_pressed_last = 0;
        }
        // Wifi combo
        static bool wifi_pressed_last = 0;
        if (cs_state.gamepad.b.pressed) {
          if (!wifi_pressed_last) {
            wifi_pressed_last = 1;
            cs_state.wifi_state = !cs_state.wifi_state;
          }
        } else {
          wifi_pressed_last = 0;
        }
      }
    }
  }

  // VMU specific
  else if (c.model == MODEL_CIRCUIT_GEM) {
    if (c.setting_input == INPUT_GPIO) {
      if (cs_state.state == STATE_MODE) {
        // OSK button pressed
        static bool debug_pressed_last = 0;
        if (cs_state.gamepad.a.pressed) {
          if (!debug_pressed_last) {
            debug_pressed_last = 1;
            cs_state.debug_state = !cs_state.debug_state;
          }
        } else {
          debug_pressed_last = 0;
        }
        // Wifi button pressed
        static bool wifi_pressed_last = 0;
        if (cs_state.gamepad.b.pressed) {
          if (!wifi_pressed_last) {
            wifi_pressed_last = 1;
            cs_state.wifi_state = !cs_state.wifi_state;
          }
        } else {
          wifi_pressed_last = 0;
        }
        // Up / Down vol
        static bool vol_pressed_last = 0;
        if (cs_state.gamepad.up.pressed) {
          if (!vol_pressed_last) {
            vol_pressed_last = 1;
            cs_state.volume += 5;
            if (cs_state.volume >= 100) {
              cs_state.volume = 100;
            }
          }
        } else if (cs_state.gamepad.down.pressed) {
          if (!vol_pressed_last) {
            vol_pressed_last = 1;
            if (cs_state.volume <= 5) {
              cs_state.volume = 0;
            } else {
              cs_state.volume -= 5;
            }
          }
        } else {
          vol_pressed_last = 0;
        }
        // On/off
        if (cs_state.gamepad.select.pressed) {
          // Shutdown now
          // do_poweroff();
          cs_state.shutdown_state = 1;
        }
        // Alt btn state
        static bool alt_pressed_last = 0;
        if (cs_state.gamepad.right.pressed) {
          if (!alt_pressed_last) {
            alt_pressed_last = 1;
            cs_state.alt_btns_state = !cs_state.alt_btns_state;
          }
        } else {
          alt_pressed_last = 0;
        }
        // External switch
        if (c.setting_vtx == ENABLED) {
          static bool ext_pressed_last = 0;
          if (cs_state.gamepad.left.pressed) {
            if (!ext_pressed_last) {
              ext_pressed_last = 1;
              cs_state.external_switch_state = !cs_state.external_switch_state;
            }
          } else {
            ext_pressed_last = 0;
          }
        }
      }
    }
  }

}

//-----------------------------------------------------------------------------

void state_unload()
{
  printf("[*] state_unload..\n");

  if (c.setting_serial == ENABLED) {
    serial_unload();
  }

  if (c.setting_batt == INPUT_DUMB_WIRE) {
    batt_mon_unload();
  }
}
