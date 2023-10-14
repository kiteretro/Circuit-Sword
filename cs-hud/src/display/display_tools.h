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

#ifndef DISPLAY_TOOLS_H
#define DISPLAY_TOOLS_H

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>

#include "imageGraphics.h"
#include "imageLayer.h"
#include "loadpng.h"

//-----------------------------------------------------------------------------
// STATIC VARIABLES

//-----------------------------------------------------------------------------
// METHODS

RGBA8_T display_int2rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void display_print_img_info(IMAGE_T *img);
void display_dump_img_to_c(IMAGE_T *img, const char* name);
void display_draw_image_on_image (IMAGE_T *canvas, IMAGE_T *image, uint16_t x, uint16_t y, bool transparent);
IMAGE_T display_load_raw_rgba(uint8_t *raw, uint32_t raw_len, int32_t width, int32_t height, VC_IMAGE_TYPE_T type);
IMAGE_T display_load_png (const char* file);
int map(int x, int x1, int x2, int y1, int y2);

//-----------------------------------------------------------------------------

#endif
