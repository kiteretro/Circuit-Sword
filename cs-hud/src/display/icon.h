
#ifndef ICON_H
#define ICON_H

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>

//-----------------------------------------------------------------------------

struct GFX_T {
  uint32_t len;
  int32_t width;
  int32_t height;
  uint32_t type;
  uint8_t data[];
};

//-----------------------------------------------------------------------------

extern struct GFX_T icons_mode_menu;
extern struct GFX_T icon_batt_chrg;
extern struct GFX_T icon_batt_long;
extern struct GFX_T icon_batt_long_sample;
extern struct GFX_T icon_batt_pwr;
extern struct GFX_T icon_brightness;
extern struct GFX_T icon_btn_a;
extern struct GFX_T icon_btn_b;
extern struct GFX_T icon_btn_down;
extern struct GFX_T icon_btn_l;
extern struct GFX_T icon_btn_left;
extern struct GFX_T icon_btn_mode;
extern struct GFX_T icon_btn_r;
extern struct GFX_T icon_btn_right;
extern struct GFX_T icon_btn_select;
extern struct GFX_T icon_btn_sleep;
extern struct GFX_T icon_btn_start;
extern struct GFX_T icon_btn_up;
extern struct GFX_T icon_btn_x;
extern struct GFX_T icon_btn_y;
extern struct GFX_T icon_flame;
extern struct GFX_T icon_kb_lower;
extern struct GFX_T icon_kb_symbols;
extern struct GFX_T icon_kb_upper;
extern struct GFX_T icon_k_logo;
extern struct GFX_T icon_k_logo_full;
extern struct GFX_T icon_mute;
extern struct GFX_T icon_temperature;
extern struct GFX_T icon_volume;
extern struct GFX_T icon_wifi_error;
extern struct GFX_T icon_wifi_sig_1;
extern struct GFX_T icon_wifi_sig_2;
extern struct GFX_T icon_wifi_sig_3;
extern struct GFX_T icon_wifi_warning;

//-----------------------------------------------------------------------------

#endif

