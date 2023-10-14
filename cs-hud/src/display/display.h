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

#ifndef DISPLAY_H
#define DISPLAY_H

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <inttypes.h>

#include "bcm_host.h"

#include "backgroundLayer.h"
#include "font.h"
#include "imageLayer.h"
#include "image.h"
#include "key.h"

#include "imageKey.h"
#include "imageGraphics.h"

#include "display_tools.h"
#include "icon.h"

//-----------------------------------------------------------------------------
// STATIC VARIABLES

extern RGBA8_T colour_black;
extern RGBA8_T colour_white;
extern RGBA8_T colour_red;
extern RGBA8_T colour_green;
extern RGBA8_T colour_blue;
extern RGBA8_T colour_clear;
extern RGBA8_T colour_bg_light;
extern RGBA8_T colour_bg_mlight;
extern RGBA8_T colour_bg_vlight;

//-----------------------------------------------------------------------------
// METHODS

void display_init(void);
void display_init_finalise(void);
void display_unload(void);

void display_set_display(uint32_t disp);

int display_get_width(void);
int display_get_height(void);
int display_get_start_x(void);
int display_get_start_y(void);
int display_get_end_x(void);
int display_get_end_y(void);

IMAGE_LAYER_T display_create_image_layer(uint32_t canvasx, uint32_t canvasy, uint32_t layer);
void display_add_image_layer(IMAGE_LAYER_T layer, uint32_t offset_x, uint32_t offset_y);

//-----------------------------------------------------------------------------

#endif
