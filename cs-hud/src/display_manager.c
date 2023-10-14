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

#include "display_manager.h"

//-----------------------------------------------------------------------------
// PRIVATE VARIABLES

// Refresh speed
uint8_t refresh_interval = REFRESH_SPEED_SLOW;
static uint8_t tick = 1;

// Top bar
IMAGE_LAYER_T battery_layer;
IMAGE_T battery_icon;
IMAGE_T battery_chrg;
IMAGE_T battery_pwr;

int batt_x = 29;
int batt_y = 1;
//int batt_bar_x_offset = 3;
//int batt_bar_y_offset = 3;
int batt_bar_height = 5;
int batt_bar_width = 22;
double batt_volt_max = 4.00;
double batt_volt_min = 3.20;
double batt_volt_low = 3.42;

int batt_chrg_x_offset = 8;
int batt_chrg_y_offset = 0;
int batt_pwr_x_offset = 8;
int batt_pwr_y_offset = 0;

// WIFI
IMAGE_T wifi_icon[WIFI_ICON_COUNT];
//int wifi_x_offset = 45;
//int wifi_y_offset = 1;

// MUTE
IMAGE_T mute_icon;
//int mute_x_offset = 60;
//int mute_y_offset = 1;

// VOLUME
IMAGE_T volume_icon;
//int volume_x_offset = 109;
//int volume_y_offset = 1;

// BRIGHTNESS
IMAGE_T brightness_icon;
//int brightness_x_offset = 160;
//int brightness_y_offset = 1;

// MODE MENU
IMAGE_T mode_menu_icons;
int mode_menu_x_offset = -64;
int mode_menu_y_offset = -16;
IMAGE_T mode_menu_batt;
IMAGE_T mode_menu_temperature;
IMAGE_T mode_menu_flame;

// ON SCREEN KEYBOARD
IMAGE_T kb_icon[KB_KBS];

// BUTTONS
IMAGE_T btn_up;
IMAGE_T btn_down;
IMAGE_T btn_left;
IMAGE_T btn_right;
IMAGE_T btn_a;
IMAGE_T btn_b;
IMAGE_T btn_x;
IMAGE_T btn_y;
IMAGE_T btn_l;
IMAGE_T btn_r;
IMAGE_T btn_start;
IMAGE_T btn_select;
IMAGE_T btn_sleep;
IMAGE_T btn_mode;

// Set the key mappings
KB_KEY_T kb_keys_map[1][KEYBOARD_KEY_COLS][KEYBOARD_KEY_ROWS] =
{ //PAGE1
  { //ROW1
    { //COL1
      { "ESC", KEY_ESC },
      { "ESC", KEY_ESC },
      { "1", KEY_1 },
      { "2", KEY_2 },
      { "3", KEY_3 },
      { "4", KEY_4 },
      { "5", KEY_5 },
      { "6", KEY_6 },
      { "7", KEY_7 },
      { "8", KEY_8 },
      { "9", KEY_9 },
      { "0", KEY_0 },
      { "BACKSPACE", KEY_BACKSPACE }
    },
    { //COL2
      { "BACKSLASH", KEY_BACKSLASH },
      { "102ND", KEY_102ND },
      { "Q", KEY_Q },
      { "W", KEY_W },
      { "E", KEY_E },
      { "R", KEY_R },
      { "T", KEY_T },
      { "Y", KEY_Y },
      { "U", KEY_U },
      { "I", KEY_I },
      { "O", KEY_O },
      { "P", KEY_P },
      { "DELETE", KEY_DELETE }
    },
    { //COL3
      { "TAB", KEY_TAB },
      { "TAB", KEY_TAB },
      { "A", KEY_A },
      { "S", KEY_S },
      { "D", KEY_D },
      { "F", KEY_F },
      { "G", KEY_G },
      { "H", KEY_H },
      { "J", KEY_J },
      { "K", KEY_K },
      { "L", KEY_L },
      { "ENTER", KEY_ENTER },
      { "ETNER", KEY_ENTER }
    },
    { //COL4
      { "LEFTSHIFT", KEY_LEFTSHIFT },
      { "LEFTSHIFT", KEY_LEFTSHIFT },
      { "Z", KEY_Z },
      { "X", KEY_X },
      { "C", KEY_C },
      { "V", KEY_V },
      { "B", KEY_B },
      { "N", KEY_N },
      { "M", KEY_M },
      { "COMMA", KEY_COMMA },
      { "DOT", KEY_DOT },
      { "SEMICOLON", KEY_SEMICOLON },
      { "SLASH", KEY_SLASH }
    },
    { //COL5
      { "LEFTCTRL", KEY_LEFTCTRL },
      { "LEFTCTRL", KEY_LEFTCTRL },
      { "LEFTALT", KEY_LEFTALT },
      { "LEFTALT", KEY_LEFTALT },
      { "SPACE", KEY_SPACE },
      { "SPACE", KEY_SPACE },
      { "SPACE", KEY_SPACE },
      { "SPACE", KEY_SPACE },
      { "SPACE", KEY_SPACE },
      { "UP", KEY_UP },
      { "DOWN", KEY_DOWN },
      { "LEFT", KEY_LEFT },
      { "RIGHT", KEY_RIGHT }
    }
  }
};

// Debug screen
IMAGE_LAYER_T debug_layer;
IMAGE_LAYER_T bg_layer;

//-----------------------------------------------------------------------------
// METHODS

void display_manager_init_resources()
{
  // Battery
  // battery_layer = display_create_image_layer(display_get_width(), display_get_height(), 99999);
  battery_layer = display_create_image_layer(display_get_width(), 30, 99998);
  display_add_image_layer(battery_layer, display_get_start_x(), display_get_start_y());

  // Load icons
  battery_icon = display_load_raw_rgba(icon_batt_long.data, icon_batt_long.len, icon_batt_long.width, icon_batt_long.height, icon_batt_long.type);
  battery_chrg = display_load_raw_rgba(icon_batt_chrg.data, icon_batt_chrg.len, icon_batt_chrg.width, icon_batt_chrg.height, icon_batt_chrg.type);
  battery_pwr  = display_load_raw_rgba(icon_batt_pwr.data, icon_batt_pwr.len, icon_batt_pwr.width, icon_batt_pwr.height, icon_batt_pwr.type);

  wifi_icon[WIFI_WARNING] = display_load_raw_rgba(icon_wifi_warning.data, icon_wifi_warning.len, icon_wifi_warning.width, icon_wifi_warning.height, icon_wifi_warning.type);
  wifi_icon[WIFI_ERROR]   = display_load_raw_rgba(icon_wifi_error.data, icon_wifi_error.len, icon_wifi_error.width, icon_wifi_error.height, icon_wifi_error.type);
  wifi_icon[WIFI_SIG_1]   = display_load_raw_rgba(icon_wifi_sig_1.data, icon_wifi_sig_1.len, icon_wifi_sig_1.width, icon_wifi_sig_1.height, icon_wifi_sig_1.type);
  wifi_icon[WIFI_SIG_2]   = display_load_raw_rgba(icon_wifi_sig_2.data, icon_wifi_sig_2.len, icon_wifi_sig_2.width, icon_wifi_sig_2.height, icon_wifi_sig_2.type);
  wifi_icon[WIFI_SIG_3]   = display_load_raw_rgba(icon_wifi_sig_3.data, icon_wifi_sig_3.len, icon_wifi_sig_3.width, icon_wifi_sig_3.height, icon_wifi_sig_3.type);

  mute_icon = display_load_raw_rgba(icon_mute.data, icon_mute.len, icon_mute.width, icon_mute.height, icon_mute.type);
  volume_icon = display_load_raw_rgba(icon_volume.data, icon_volume.len, icon_volume.width, icon_volume.height, icon_volume.type);
  brightness_icon = display_load_raw_rgba(icon_brightness.data, icon_brightness.len, icon_brightness.width, icon_brightness.height, icon_brightness.type);

  mode_menu_icons = display_load_raw_rgba(icons_mode_menu.data, icons_mode_menu.len, icons_mode_menu.width, icons_mode_menu.height, icons_mode_menu.type);
  mode_menu_batt = display_load_raw_rgba(icon_batt_long_sample.data, icon_batt_long_sample.len, icon_batt_long_sample.width, icon_batt_long_sample.height, icon_batt_long_sample.type);
  mode_menu_temperature = display_load_raw_rgba(icon_temperature.data, icon_temperature.len, icon_temperature.width, icon_temperature.height, icon_temperature.type);
  mode_menu_flame = display_load_raw_rgba(icon_flame.data, icon_flame.len, icon_flame.width, icon_flame.height, icon_flame.type);

  kb_icon[KB_LOWER] = display_load_raw_rgba(icon_kb_lower.data, icon_kb_lower.len, icon_kb_lower.width, icon_kb_lower.height, icon_kb_lower.type);
  kb_icon[KB_UPPER] = display_load_raw_rgba(icon_kb_upper.data, icon_kb_upper.len, icon_kb_upper.width, icon_kb_upper.height, icon_kb_upper.type);

  btn_up = display_load_raw_rgba(icon_btn_up.data, icon_btn_up.len, icon_btn_up.width, icon_btn_up.height, icon_btn_up.type);
  btn_down = display_load_raw_rgba(icon_btn_down.data, icon_btn_down.len, icon_btn_down.width, icon_btn_down.height, icon_btn_down.type);
  btn_left = display_load_raw_rgba(icon_btn_left.data, icon_btn_left.len, icon_btn_left.width, icon_btn_left.height, icon_btn_left.type);
  btn_right = display_load_raw_rgba(icon_btn_right.data, icon_btn_right.len, icon_btn_right.width, icon_btn_right.height, icon_btn_right.type);
  btn_a = display_load_raw_rgba(icon_btn_a.data, icon_btn_a.len, icon_btn_a.width, icon_btn_a.height, icon_btn_a.type);
  btn_b = display_load_raw_rgba(icon_btn_b.data, icon_btn_b.len, icon_btn_b.width, icon_btn_b.height, icon_btn_b.type);
  btn_x = display_load_raw_rgba(icon_btn_x.data, icon_btn_x.len, icon_btn_x.width, icon_btn_x.height, icon_btn_x.type);;
  btn_y = display_load_raw_rgba(icon_btn_y.data, icon_btn_y.len, icon_btn_y.width, icon_btn_y.height, icon_btn_y.type);;
  btn_l = display_load_raw_rgba(icon_btn_l.data, icon_btn_l.len, icon_btn_l.width, icon_btn_l.height, icon_btn_l.type);;
  btn_r = display_load_raw_rgba(icon_btn_r.data, icon_btn_r.len, icon_btn_r.width, icon_btn_r.height, icon_btn_r.type);;

  btn_start = display_load_raw_rgba(icon_btn_start.data, icon_btn_start.len, icon_btn_start.width, icon_btn_start.height, icon_btn_start.type);
  btn_select = display_load_raw_rgba(icon_btn_select.data, icon_btn_select.len, icon_btn_select.width, icon_btn_select.height, icon_btn_select.type);
  btn_sleep = display_load_raw_rgba(icon_btn_sleep.data, icon_btn_sleep.len, icon_btn_sleep.width, icon_btn_sleep.height, icon_btn_sleep.type);
  btn_mode = display_load_raw_rgba(icon_btn_mode.data, icon_btn_mode.len, icon_btn_mode.width, icon_btn_mode.height, icon_btn_mode.type);

  // Debug
  debug_layer = display_create_image_layer(display_get_width(), display_get_height(), 99997);
  display_add_image_layer(debug_layer, display_get_start_x(), display_get_start_y());

  // Background
  bg_layer = display_create_image_layer(display_get_width(), display_get_height(), 99996);
  display_add_image_layer(bg_layer, display_get_start_x(), display_get_start_y());
}

//-----------------------------------------------------------------------------

int draw_battery(int batt_bar_x_offset, int batt_bar_y_offset)
{
  int width = 0;

  // Get voltage
  double voltage = cs_state.batt_voltage;

  // Hide battery if enabled
  if (c.hide_battery) {
    if (voltage > c.hide_battery_val) {
      return width;
    }
  }

  width = 28;

  // Get the canvas/image
  IMAGE_T *image = &(battery_layer.image);

  // Draw battery bar fill bg
  imageBoxFilledRGB(
    image,
    display_get_width() - batt_x + batt_bar_x_offset,
    batt_y + batt_bar_y_offset,
    display_get_width() - batt_x + batt_bar_width + 2,
    batt_y + batt_bar_height + 2,
    &colour_black
  );

  // Draw battery bar
  int batt_current_width = map((int)(voltage*100), (int)(batt_volt_min*100), (int)(batt_volt_max*100), 0, batt_bar_width);

  if (voltage > batt_volt_min) {
    RGBA8_T battColour;
    if (voltage > batt_volt_low)
    {
      battColour = colour_green;
    } else {
      battColour = colour_red;
    }

    // Draw the bar
    imageBoxFilledRGB(
      image,
      display_get_width() - batt_x + batt_bar_x_offset,
      batt_y + batt_bar_y_offset,
      display_get_width() - batt_x + batt_current_width + 2,
      batt_y + batt_bar_height + 2,
      &battColour
    );
  }

  // Draw the battery icon
  display_draw_image_on_image(
    image,
    &battery_icon,
    display_get_width() - batt_x,
    batt_y,
    false
  );

  // Draw charge state
  if (cs_state.chrg_state) {
    display_draw_image_on_image(
      image,
      &battery_chrg,
      display_get_width() - batt_x + batt_chrg_x_offset,
      batt_y + batt_chrg_y_offset,
      false
    );
  } else if (cs_state.pg_state) {
    display_draw_image_on_image(
      image,
      &battery_pwr,
      display_get_width() - batt_x + batt_pwr_x_offset,
      batt_y + batt_pwr_y_offset,
      false
    );
  }

  return width;
}

int draw_wifi(int wifi_x_offset, int wifi_y_offset)
{
  int width = 0;

  // Wifi icon
  if (cs_state.wifi_state) {
    width = 15;
    // Get the canvas/image
    IMAGE_T *image = &(battery_layer.image);
    // Draw to it
    display_draw_image_on_image(
      image,
      &wifi_icon[cs_state.wifi_status],
      display_get_width() - wifi_x_offset - wifi_icon[cs_state.wifi_status].width,
      wifi_y_offset,
      false
    );
  }

  return width;
}

int draw_mute(int mute_x_offset, int mute_y_offset)
{
  int width = 0;

  // Mute icon
  if (cs_state.mute_state) {
    width = 15;
    // Get the canvas/image
    IMAGE_T *image = &(battery_layer.image);
    // Draw to it
    display_draw_image_on_image(
      image,
      &mute_icon,
      display_get_width() - mute_x_offset - mute_icon.width,
      mute_y_offset,
      false
    );
  }

  return width;
}

int draw_volume(int volume_x_offset, int volume_y_offset)
{
  int width = 0;

  // VOL CHANGE
  static int8_t volume_last = -1;
  static uint8_t hold_counter = 0;
  static uint8_t start_counter = 10;

  if (start_counter > ICON_START_COUNT && cs_state.state != STATE_MODE) {
    if (cs_state.volume <= volume_last - 2 || cs_state.volume >= volume_last + 2 || hold_counter > 0) {
      // Get the canvas/image
      IMAGE_T *image = &(battery_layer.image);

      // Determine text offset
      width = 16;
      if (cs_state.volume < 10) {
        width += (8 * 2);
      } else if (cs_state.volume < 100) {
        width += (8 * 3);
      } else {
        width += (8 * 4);
      }

      // Draw volume
      char buffer[32];
      snprintf(buffer, sizeof(buffer), "%i%%", cs_state.volume);
      drawStringRGB(display_get_width() - volume_x_offset + 15 - width, volume_y_offset - 2, buffer, &colour_white, image);

      // Draw icon
      display_draw_image_on_image(
        image,
        &volume_icon,
        display_get_width() - volume_x_offset - width,
        volume_y_offset,
        false
      );

      // Update hold counter
      if (cs_state.volume != volume_last) {
        hold_counter = ICON_HOLD_COUNT;
        volume_last = cs_state.volume;
      } else {
        hold_counter--;
      }
    }
  } else {
    volume_last = cs_state.volume;
    start_counter++;
    hold_counter = 0;
  }

  return width;
}

int draw_brightness(int brightness_x_offset, int brightness_y_offset)
{
  int width = 0;

  // BL CHANGE
  static int8_t brightness_last = -1;
  static uint8_t hold_counter = 0;
  if (cs_state.brightness != brightness_last || hold_counter > 0) {
    width = 15;
  // if (1) {
    // Get the canvas/image
    IMAGE_T *image = &(battery_layer.image);

    // Draw brightness
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%i%%", cs_state.brightness);
    drawStringRGB(display_get_width() - brightness_x_offset + 17 , brightness_y_offset - 2, buffer, &colour_white, image);

    // Draw icon
    display_draw_image_on_image(
      image,
      &brightness_icon,
      display_get_width() - brightness_x_offset,
      brightness_y_offset,
      false
    );

    // Update hold counter
    if (cs_state.brightness != brightness_last) {
      hold_counter = ICON_HOLD_COUNT;
      brightness_last = cs_state.brightness;
    } else {
      hold_counter--;
    }
  }

  return width;
}

void draw_debug()
{
  // Get the canvas/image
  IMAGE_T *image = &(debug_layer.image);

  // Draw some text info
  imageBoxFilledRGB(image, 18, 8, 120, 160, &colour_black);

  char buffer[32];
  uint16_t y_start = 10;
  uint16_t line = 0;

  snprintf(buffer, sizeof(buffer), "POWER: %i", cs_state.power_switch_on);
  drawStringRGB(20 , y_start + (line++ * 11), buffer, &colour_white, image);

  snprintf(buffer, sizeof(buffer), "CHRG:  %i", cs_state.chrg_state);
  drawStringRGB(20 , y_start + (line++ * 11), buffer, &colour_white, image);

  snprintf(buffer, sizeof(buffer), "PG:    %i", cs_state.pg_state);
  drawStringRGB(20 , y_start + (line++ * 11), buffer, &colour_white, image);

  snprintf(buffer, sizeof(buffer), "VOLT : %.2fv", cs_state.batt_voltage);
  drawStringRGB(20 , y_start + (line++ * 11), buffer, &colour_white, image);

  snprintf(buffer, sizeof(buffer), "TEMP : %.1fC", cs_state.temperature);
  drawStringRGB(20 , y_start + (line++ * 11), buffer, &colour_white, image);

  snprintf(buffer, sizeof(buffer), "BL:    %i%%", cs_state.brightness);
  drawStringRGB(20 , y_start + (line++ * 11), buffer, &colour_white, image);

  snprintf(buffer, sizeof(buffer), "VOL:   %i%%", cs_state.volume);
  drawStringRGB(20 , y_start + (line++ * 11), buffer, &colour_white, image);

  snprintf(buffer, sizeof(buffer), "MODE:  %i", cs_state.mode_button_on);
  drawStringRGB(20 , y_start + (line++ * 11), buffer, &colour_white, image);

  snprintf(buffer, sizeof(buffer), "WIFI:  %i", cs_state.wifi_state);
  drawStringRGB(20 , y_start + (line++ * 11), buffer, &colour_white, image);

  snprintf(buffer, sizeof(buffer), "MUTE:  %i", cs_state.mute_state);
  drawStringRGB(20 , y_start + (line++ * 11), buffer, &colour_white, image);

  snprintf(buffer, sizeof(buffer), "DEBUG: %i", cs_state.debug_state);
  drawStringRGB(20 , y_start + (line++ * 11), buffer, &colour_white, image);
}

//-----------------------------------------------------------------------------

void draw_kb()
{
  // Get the canvas/image
  IMAGE_T *image = &(debug_layer.image);

  // Mid point of icons to base everything else from
  int x_start = display_get_width()/2 - KEYBOARD_WIDTH/2 + KEYBOARD_X_OFFSET;
  int y_start = display_get_height() - KEYBOARD_HEIGHT - KEYBOARD_Y_OFFSET;

  // Set cursor details
  uint16_t cursor_start_x = x_start + KEYBOARD_KEY_X_OFFSET;
  uint16_t cursor_start_y = y_start + KEYBOARD_KEY_Y_OFFSET;
  static uint8_t cursor_x = 2;
  static uint8_t cursor_y = 1;
  static uint8_t cursor_page = KB_LOWER;
  static bool is_shift = false;
  static bool is_ctrl = false;
  static bool is_alt = false;

  // Read input
  static uint8_t right_counter = 0;
  if (cs_state.gamepad.right.pressed) {
    if (right_counter == 0 || right_counter > 1) {
      cursor_x++;
      if (cursor_x >= KEYBOARD_KEY_ROWS) {
        cursor_x = 0;
      }
    }
    if (right_counter < 5) {
      right_counter++;
    }
  } else {
    right_counter = 0;
  }

  static uint8_t left_counter = 0;
  if (cs_state.gamepad.left.pressed) {
    if (left_counter == 0 || left_counter > 1) {
      if (cursor_x == 0) {
        cursor_x = KEYBOARD_KEY_ROWS - 1;
      } else {
        cursor_x--;
      }
    }
    if (left_counter < 5) {
      left_counter++;
    }
  } else {
    left_counter = 0;
  }

  static uint8_t down_counter = 0;
  if (cs_state.gamepad.down.pressed) {
    if (down_counter == 0 || down_counter > 1) {
      cursor_y++;
      if (cursor_y >= KEYBOARD_KEY_COLS) {
        cursor_y = 0;
      }
    }
    if (down_counter < 5) {
      down_counter++;
    }
  } else {
    down_counter = 0;
  }

  static uint8_t up_counter = 0;
  if (cs_state.gamepad.up.pressed) {
    if (up_counter == 0 || up_counter > 1) {
      if (cursor_y == 0) {
        cursor_y = KEYBOARD_KEY_COLS - 1;
      } else {
        cursor_y--;
      }
    }
    if (up_counter < 5) {
      up_counter++;
    }
  } else {
    up_counter = 0;
  }

  // KB BG
  imageBoxFilledRGB(
    image,
    x_start,
    y_start,
    x_start + KEYBOARD_WIDTH - 1,
    y_start + KEYBOARD_HEIGHT - 1,
    &colour_bg_vlight
  );

  // Modifiers
  if (is_shift) {
    imageBoxFilledRGB(
      image,
      cursor_start_x + (KEYBOARD_KEY_WIDTH * 0) + 0,
      cursor_start_y + (KEYBOARD_KEY_HEIGHT * 3) + 3,
      cursor_start_x + (KEYBOARD_KEY_WIDTH * (1 + 1)) + 1,
      cursor_start_y + (KEYBOARD_KEY_HEIGHT * (3 + 1)) + 3,
      &colour_blue
    );
  }
  if (is_ctrl) {
    imageBoxFilledRGB(
      image,
      cursor_start_x + (KEYBOARD_KEY_WIDTH * 0) + 0,
      cursor_start_y + (KEYBOARD_KEY_HEIGHT * 4) + 4,
      cursor_start_x + (KEYBOARD_KEY_WIDTH * (1 + 1)) + 1,
      cursor_start_y + (KEYBOARD_KEY_HEIGHT * (4 + 1)) + 4,
      &colour_blue
    );
  }
  if (is_alt) {
    imageBoxFilledRGB(
      image,
      cursor_start_x + (KEYBOARD_KEY_WIDTH * 2) + 2,
      cursor_start_y + (KEYBOARD_KEY_HEIGHT * 4) + 4,
      cursor_start_x + (KEYBOARD_KEY_WIDTH * (3 + 1)) + 2,
      cursor_start_y + (KEYBOARD_KEY_HEIGHT * (4 + 1)) + 4,
      &colour_blue
    );
  }

  // Draw cursor
  if (cs_state.gamepad.a.pressed) {
    imageBoxFilledRGB(
      image,
      cursor_start_x + (KEYBOARD_KEY_WIDTH * cursor_x) + cursor_x,
      cursor_start_y + (KEYBOARD_KEY_HEIGHT * cursor_y) + cursor_y,
      cursor_start_x + (KEYBOARD_KEY_WIDTH * (cursor_x + 1)) + cursor_x,
      cursor_start_y + (KEYBOARD_KEY_HEIGHT * (cursor_y + 1)) + cursor_y,
      &colour_red
    );
  } else {
    imageBoxFilledRGB(
      image,
      cursor_start_x + (KEYBOARD_KEY_WIDTH * cursor_x) + cursor_x,
      cursor_start_y + (KEYBOARD_KEY_HEIGHT * cursor_y) + cursor_y,
      cursor_start_x + (KEYBOARD_KEY_WIDTH * (cursor_x + 1)) + cursor_x,
      cursor_start_y + (KEYBOARD_KEY_HEIGHT * (cursor_y + 1)) + cursor_y,
      &colour_green
    );
  }

  // Draw the KB
  display_draw_image_on_image(
    image,
    &kb_icon[is_shift],
    x_start,
    y_start,
    false
  );

  // Press the keys
  static uint8_t lastPressed = 0;
  static uint8_t lastPressedHoldCount = 2;
  if (cs_state.gamepad.a.pressed) {
    if (!lastPressed) {
      printf("[d] PAGE=[%i] ROW=[%i COL=[%i] KEY=[%s]\n", cursor_page, cursor_y, cursor_x, kb_keys_map[cursor_page][cursor_y][cursor_x].name);

      // Special HOLD keys setting
      if (kb_keys_map[cursor_page][cursor_y][cursor_x].value == KEY_LEFTSHIFT) {
        is_shift = !is_shift;
      } else if (kb_keys_map[cursor_page][cursor_y][cursor_x].value == KEY_LEFTCTRL) {
        is_ctrl = !is_ctrl;
      } else if (kb_keys_map[cursor_page][cursor_y][cursor_x].value == KEY_LEFTALT) {
        is_alt = !is_alt;
      } else {
        // Real key press
        if (is_shift)
          keyboard_press(KEY_LEFTSHIFT);
        if (is_ctrl)
          keyboard_press(KEY_LEFTCTRL);
        if (is_alt)
          keyboard_press(KEY_LEFTALT);

        // PRESS!
        keyboard_button(kb_keys_map[cursor_page][cursor_y][cursor_x].value);

        if (is_shift)
          keyboard_release(KEY_LEFTSHIFT);
        if (is_ctrl)
          keyboard_release(KEY_LEFTCTRL);
        if (is_alt)
          keyboard_release(KEY_LEFTALT);
      }
    }

    lastPressed++;

    if (lastPressed > lastPressedHoldCount) {
      lastPressed = 0;
    }
  } else {
    lastPressed = 0;
  }
}

//-----------------------------------------------------------------------------

void draw_mode_info()
{
  // Get the canvas/image
  IMAGE_T *image = &(debug_layer.image);
  IMAGE_T *bg = &(bg_layer.image);

  // BG is grey
  clearImageRGB(bg, &colour_bg_light);

  // Determine mid/starting points
  int x_mid = display_get_width()/2;
  int y_mid = display_get_height()/2;

  // Draw text
  char buffer[32];
  uint16_t x_start = x_mid;
  uint16_t y_start = y_mid;
  uint16_t line = 0;
  uint16_t line_height = 26;

  if (c.model == MODEL_CIRCUIT_SWORD) {
    x_start -= 100;
    y_start -= 95;
    line_height = 21;
  } else if (c.model == MODEL_CIRCUIT_SWORD_LITE) {
    x_start -= 75;
    y_start -= 50;
  } else if (c.model == MODEL_CIRCUIT_GEM) {
    x_start -= 85;
    y_start -= 85;
    line_height = 21;
  }

  if (c.model == MODEL_CIRCUIT_GEM) {
    // VMU
    // Volume
    display_draw_image_on_image(image, &btn_up, x_start, y_start + (line * line_height), true);
    display_draw_image_on_image(image, &btn_down, x_start + 16 + 5, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "VOLUME (%i%%)", cs_state.volume);
    drawStringRGB(x_start + 16 + 5 + 16 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Button switch
    display_draw_image_on_image(image, &btn_right, x_start, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "SLP/MODE AS X/Y (%s)", (cs_state.alt_btns_state)?"ON":"OFF");
    drawStringRGB(x_start + 16 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Ext switch
    if (c.setting_vtx == ENABLED) {
      display_draw_image_on_image(image, &btn_left, x_start, y_start + (line * line_height), true);
      snprintf(buffer, sizeof(buffer), "INTERNAL SWITCH (%s)", (cs_state.external_switch_state)?"ON":"OFF");
      drawStringRGB(x_start + 16 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);
    }

    // OSK
    display_draw_image_on_image(image, &btn_a, x_start, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "KEYBOARD (%s)", (cs_state.debug_state)?"ON":"OFF");
    drawStringRGB(x_start + 16 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Wifi
    display_draw_image_on_image(image, &btn_b, x_start, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "WIFI (%s)", (cs_state.wifi_state)?"ON":"OFF");
    drawStringRGB(x_start + 16 + 5, y_start + 1 + (line++ * line_height), buffer, &colour_white, image);
    if (cs_state.wifi_state) {
      snprintf(buffer, sizeof(buffer), "(%i%%)", cs_state.wifi_signal);
      drawStringRGB(x_start + 16 + 5 + 80 , y_start + 1 + ((line - 1) * line_height), buffer, &colour_white, image);
    }

    // Shutdown
    display_draw_image_on_image(image, &btn_sleep, x_start, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "SAFE SHUTDOWN");
    drawStringRGB(x_start + 57 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Estop
    display_draw_image_on_image(image, &btn_mode, x_start, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "EMERGENCY OFF");
    drawStringRGB(x_start + 57 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Batt
    display_draw_image_on_image(image, &mode_menu_batt, x_start, y_start + (line * line_height), false);
    snprintf(buffer, sizeof(buffer), "%.2fV", cs_state.batt_voltage);
    drawStringRGB(x_start + 17 + 5, y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Temp
    display_draw_image_on_image(image, &mode_menu_temperature, x_start, y_start + 1 + (line * line_height), false);
    snprintf(buffer, sizeof(buffer), "%.1fC", cs_state.temperature);
    drawStringRGB(x_start + 13 + 5, y_start + 1 + (line++ * line_height), buffer, &colour_white, image);
    if (cs_state.temperature_over) {
      display_draw_image_on_image(image, &mode_menu_flame, x_start + 13 + 5 + 45 , y_start + ((line - 1) * line_height), false);
    }

  } else if (c.model == MODEL_CIRCUIT_SWORD_LITE) {
    // LITE
    // Wifi
    display_draw_image_on_image(image, &btn_b, x_start, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "WIFI (%s)", (cs_state.wifi_state)?"ON":"OFF");
    drawStringRGB(x_start + 16 + 5, y_start + 1 + (line++ * line_height), buffer, &colour_white, image);
    if (cs_state.wifi_state) {
      snprintf(buffer, sizeof(buffer), "(%i%%)", cs_state.wifi_signal);
      drawStringRGB(x_start + 16 + 5 + 80 , y_start + 1 + ((line - 1) * line_height), buffer, &colour_white, image);
    }

    // OSK
    display_draw_image_on_image(image, &btn_a, x_start, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "KEYBOARD (%s)", (cs_state.debug_state)?"ON":"OFF");
    drawStringRGB(x_start + 16 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Batt
    display_draw_image_on_image(image, &mode_menu_batt, x_start, y_start + (line * line_height), false);
    snprintf(buffer, sizeof(buffer), "%.2fV %s", cs_state.batt_voltage, (cs_state.chrg_state)?"(CHARGING)":"");
    drawStringRGB(x_start + 17 + 5, y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Temp
    display_draw_image_on_image(image, &mode_menu_temperature, x_start, y_start + 1 + (line * line_height), false);
    snprintf(buffer, sizeof(buffer), "%.1fC", cs_state.temperature);
    drawStringRGB(x_start + 13 + 5, y_start + 1 + (line++ * line_height), buffer, &colour_white, image);
    if (cs_state.temperature_over) {
      display_draw_image_on_image(image, &mode_menu_flame, x_start + 13 + 5 + 45 , y_start + ((line - 1) * line_height), false);
    }
  } else if (c.model == MODEL_CIRCUIT_SWORD) {
    // CS
    // Volume
    display_draw_image_on_image(image, &btn_up, x_start, y_start + (line * line_height), true);
    display_draw_image_on_image(image, &btn_down, x_start + 16 + 5, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "VOLUME (%i%%)", cs_state.volume);
    drawStringRGB(x_start + 16 + 5 + 16 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Brightness
    display_draw_image_on_image(image, &btn_left, x_start, y_start + (line * line_height), true);
    display_draw_image_on_image(image, &btn_right, x_start + 16 + 5, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "BRIGHTNESS (%i%%)", cs_state.brightness);
    drawStringRGB(x_start + 16 + 5 + 16 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Wifi
    display_draw_image_on_image(image, &btn_b, x_start, y_start + (line * line_height), true);
    display_draw_image_on_image(image, &btn_a, x_start + 16 + 5, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "WIFI (%s)", (cs_state.wifi_state)?"ON":"OFF");
    drawStringRGB(x_start + 16 + 5 + 16 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);
    if (cs_state.wifi_state) {
      snprintf(buffer, sizeof(buffer), "(%i%%)", cs_state.wifi_signal);
      drawStringRGB(x_start + 16 + 5 + 16 + 5 + 80 , y_start + 1 + ((line - 1) * line_height), buffer, &colour_white, image);
    }

    // AMP
    display_draw_image_on_image(image, &btn_y, x_start, y_start + (line * line_height), true);
    display_draw_image_on_image(image, &btn_x, x_start + 16 + 5, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "SPEAKER (%s)", (cs_state.mute_state)?"OFF":"ON");
    drawStringRGB(x_start + 16 + 5 + 16 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Joy as DPAD
    display_draw_image_on_image(image, &btn_l, x_start, y_start + (line * line_height), true);
    display_draw_image_on_image(image, &btn_r, x_start + 16 + 5, y_start + (line * line_height), true);
    snprintf(buffer, sizeof(buffer), "JOYSTICK D-PAD (%s)", (cs_state.dpad_btns_state)?"ON":"OFF");
    drawStringRGB(x_start + 16 + 5 + 16 + 5 , y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // OSK
    display_draw_image_on_image(image, &btn_start, x_start, y_start + (line * line_height), true);
    display_draw_image_on_image(image, &btn_select, x_start, y_start + ((line + 1) * line_height), true);
    snprintf(buffer, sizeof(buffer), "ON SCREEN");
    drawStringRGB(x_start + 57 + 5 , y_start  + 5 + (line++ * line_height), buffer, &colour_white, image);
    snprintf(buffer, sizeof(buffer), "KEYBOARD (%s)", (cs_state.debug_state)?"ON":"OFF");
    drawStringRGB(x_start + 57 + 5 , y_start - 3 + (line++ * line_height), buffer, &colour_white, image);

    // Batt
    display_draw_image_on_image(image, &mode_menu_batt, x_start, y_start + (line * line_height), false);
    snprintf(buffer, sizeof(buffer), "%.2fV %s", cs_state.batt_voltage, (cs_state.chrg_state)?"(CHARGING)":"");
    drawStringRGB(x_start + 17 + 5, y_start + 1 + (line++ * line_height), buffer, &colour_white, image);

    // Temp
    display_draw_image_on_image(image, &mode_menu_temperature, x_start, y_start + 1 + (line * line_height), false);
    snprintf(buffer, sizeof(buffer), "%.1fC", cs_state.temperature);
    drawStringRGB(x_start + 13 + 5, y_start + 1 + (line++ * line_height), buffer, &colour_white, image);
    if (cs_state.temperature_over) {
      display_draw_image_on_image(image, &mode_menu_flame, x_start + 13 + 5 + 45 , y_start + ((line - 1) * line_height), false);
    }

  }

}

//-----------------------------------------------------------------------------

void process_top_bar()
{
  clearImageRGB(&(battery_layer.image), &colour_clear); // Clear canvas

  uint16_t pos = 3;
  pos += draw_battery(pos, 3);
  pos += draw_wifi(pos, 1);
  pos += draw_mute(pos, 1);
  pos += draw_volume(pos, 1);
  //pos += draw_brightness(pos, 3);

  changeSourceAndUpdateImageLayer(&battery_layer); // Update
}

//-----------------------------------------------------------------------------

void process_full_screen()
{
  // When not showing anything, don't waste time drawing
  static bool cleared = false;

  // Should it be cleared
  if (cs_state.state == STATE_NONE) {
    if (!cleared) {
      clearImageRGB(&(debug_layer.image), &colour_clear);
      clearImageRGB(&(bg_layer.image), &colour_clear);
      changeSourceAndUpdateImageLayer(&debug_layer);
      changeSourceAndUpdateImageLayer(&bg_layer);
      cleared = true;
    }
    return;
  }

  // We should be displaying something, so let's start!
  clearImageRGB(&(debug_layer.image), &colour_clear);
  clearImageRGB(&(bg_layer.image), &colour_clear);
  cleared = false;

  // Should show mode info
  if (cs_state.state == STATE_MODE) {

    draw_mode_info();

  // Should show OSK?
  } else if (cs_state.state == STATE_OSK) {

    // draw_debug();
    draw_kb();

  } else {
    printf("[!] Invalid display state (action): %i", cs_state.state);
  }

  // Should show menu?

  // Should show debug?

  // Update changes to screen now
  changeSourceAndUpdateImageLayer(&debug_layer);
  changeSourceAndUpdateImageLayer(&bg_layer);

}

//-----------------------------------------------------------------------------

void display_manager_set_refresh_speed(uint8_t speed)
{
  if (speed != refresh_interval) {
    printf("[d] Setting display_manager refresh speed to [%i]\n", speed);
    refresh_interval = speed;
  }
}

//-----------------------------------------------------------------------------

void display_manager_process()
{

  // Set drawing speed
  if (cs_state.state != STATE_NONE) {
    display_manager_set_refresh_speed(REFRESH_SPEED_FAST);
  } else {
    display_manager_set_refresh_speed(REFRESH_SPEED_SLOW);
  }

  // Drawing speed limiter
  if (tick % refresh_interval == 0) {

    // Draw top bar
    process_top_bar();

    // Draw debug info
    // process_debug();

    // Draw anything full screen
    process_full_screen();

  }

  // Increment tick
  tick++;
  if (tick > REFRESH_SPEED_SLOW) {
    tick = 1;
  }

}

//-----------------------------------------------------------------------------

void display_manager_init()
{
  printf("[i] display_manager_init..\n");

  // Configure display
  display_init();

  // Configure resources
  display_manager_init_resources();

  // Finalise the display (no more new layers from here on out)
  display_init_finalise();
}

//-----------------------------------------------------------------------------

void display_manager_unload()
{
  printf("[i] display_manager_unload..\n");

  display_unload();
}

void display_manager_clear()
{
  clearImageRGB(&(debug_layer.image), &colour_clear);
  clearImageRGB(&(bg_layer.image), &colour_clear);
  changeSourceAndUpdateImageLayer(&debug_layer);
  changeSourceAndUpdateImageLayer(&bg_layer);
}

//-----------------------------------------------------------------------------
