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

#include "display_tools.h"

//-----------------------------------------------------------------------------
// METHODS

// Make a colour from ints
RGBA8_T display_int2rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  RGBA8_T colour;

  colour.red = r;
  colour.green = g;
  colour.blue = b;
  colour.alpha = a;

  return colour;
}

//-----------------------------------------------------------------------------

// Display some info
void display_print_img_info(IMAGE_T *img)
{
  printf("type: %d\n", img->type);
  printf("width: %d\n", img->width);
  printf("height: %d\n", img->height);
  printf("pitch: %d\n", img->pitch);
  printf("alignedHeight: %d\n", img->alignedHeight);
  printf("bitsPerPixel: %d\n", img->bitsPerPixel);
  printf("size: %d\n", img->size);

  uint8_t *x;
  x = (uint8_t *)img->buffer;

  int pos;
  printf("uint8_t data[%d] = { ", img->size);
  for (pos = 0; pos < img->size ; pos++) {
     printf("0x%02x, ", x[pos]);
  }
  printf("};\n");
}

// Convert an image into something we can put in a .c/.h file
void display_dump_img_to_c(IMAGE_T *img, const char* name)
{
  /*
  struct GFX_T icon_k_logo = {
    .len = 35840,
    .width = 100,
    .height = 80,
    .type = 15,
    .data = {}
  };
  extern struct GFX_T icon_k_logo;
  */

  printf("\nHEADER\n------\n");
  printf("extern struct GFX_T %s;\n", name);

  printf("\n");

  printf("SOURCE\n------\n");
  printf("struct GFX_T %s = {\n", name);
  printf("  .len    = %d,\n", img->size);
  printf("  .width  = %d,\n", img->width);
  printf("  .height = %d,\n", img->height);
  printf("  .type   = %d,\n", img->type);

  uint8_t *ptr;
  ptr = (uint8_t *)img->buffer;

  printf("  .data   = {\n    ");
  int pos = 0;
  int counter = 1;
  for (pos = 0; pos < img->size ; pos++) {

    printf("0x%02x", ptr[pos]);

    if (counter == img->size) {
      printf("\n");
    } else {
      printf(", ");

      if (counter % 16 == 0) {
        printf("\n    ");
      }
    }

    counter++;
  }
  printf("  }\n};\n");
}

//-----------------------------------------------------------------------------

// Draw image on image
void display_draw_image_on_image(IMAGE_T *canvas, IMAGE_T *image, uint16_t x, uint16_t y, bool transparent)
{
  uint32_t a = 0;
  uint32_t b = 0;
  if (transparent) {
    for (b = 0; b < image->height; b++) {
      for (a = 0; a < image->width; a++) {
        RGBA8_T pix;
        getPixelRGB(image, a, b, &pix);
        if (pix.alpha > 0) {
          setPixelRGB(canvas, a+x, b+y, &pix);
        }
      }
    }
  } else {
    for (b = 0; b < image->height; b++) {
      for (a = 0; a < image->width; a++) {
        RGBA8_T pix;
        getPixelRGB(image, a, b, &pix);
        if (pix.alpha == 255) {
          setPixelRGB(canvas, a+x, b+y, &pix);
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------

// Load a raw RGBA image (IMAGE_T ii = display_load_raw_rgba(data, data_len, 28, 11, 15);)
IMAGE_T display_load_raw_rgba(uint8_t *raw, uint32_t raw_len, int32_t width, int32_t height, VC_IMAGE_TYPE_T type)
{
  // Blank object
  IMAGE_T img;

  // Init the object
  initImage(&img, type, width, height, false);

  // Copy contents of raw data into buffer
  memcpy(img.buffer, raw, raw_len);

  fprintf(stdout, "[i] Loaded image with dimensions X=%i Y=%i\n", img.width, img.height);

  // Return completed object
  return img;
}

//-----------------------------------------------------------------------------

// Load a png
IMAGE_T display_load_png (const char* file)
{
  IMAGE_T img;

  bool loaded = loadPng(&img, file);
  if (!loaded) {
    fprintf(stderr, "[!] ERROR: Failed to load png [%s]\n", file);
    exit(2);
  }

  fprintf(stdout, "[i] Loaded png [%s] with dimensions X=%i Y=%i\n", file, img.width, img.height);

  // display_print_img_info(&img);

  return img;
}

//-----------------------------------------------------------------------------

// map(value, fromLow, fromHigh, toLow, toHigh)
int map(int x, int x1, int x2, int y1, int y2)
{
  //return (x - x1) * (y2 - y1) / (x2 - x1) + y1;
  int val = (x - x1) * (y2 - y1) / (x2 - x1) + y1;
  if (val < y1)
  {
    return y1;
  }
  else if (val > y2)
  {
    return y2;
  }
  else
  {
    return val;
  }
}

//-----------------------------------------------------------------------------
