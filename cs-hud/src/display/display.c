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

#include "display.h"

//-----------------------------------------------------------------------------
// PRIVATE VARIABLES

DISPMANX_DISPLAY_HANDLE_T display;
DISPMANX_MODEINFO_T info;
DISPMANX_UPDATE_HANDLE_T update;

IMAGE_LAYER_T base_layer;

uint32_t display_number = 0;

RGBA8_T colour_black;
RGBA8_T colour_white;
RGBA8_T colour_red;
RGBA8_T colour_green;
RGBA8_T colour_blue;
RGBA8_T colour_clear;
RGBA8_T colour_bg_light;
RGBA8_T colour_bg_mlight;
RGBA8_T colour_bg_vlight;

//-----------------------------------------------------------------------------
// METHODS

// Get width of screen
int display_get_width()
{
  return info.width;
}

// Get height of screen
int display_get_height()
{
  return info.height;
}

// Get start posx of screen
int display_get_start_x()
{
  return 0;
}

// Get start posy of screen
int display_get_start_y()
{
  return 0;
}

// Get end posx of screen
int display_get_end_x()
{
  return display_get_width() - display_get_start_x();
}

// Get end posx of screen
int display_get_end_y()
{
  return display_get_height() - display_get_start_y();
}

//-----------------------------------------------------------------------------

// Draw splash screen
void draw_splash()
{
  // Get the canvas/image
  IMAGE_T *image = &(base_layer.image);

  // Clear canvas
  clearImageRGB(image, &colour_bg_mlight);

  /* ORIGINAL
  // Load image
  IMAGE_T icon = display_load_raw_rgba(icon_k_logo.data,icon_k_logo.len, icon_k_logo.width, icon_k_logo.height, icon_k_logo.type);
  display_draw_image_on_image(image, &icon, (display_get_width() - icon_k_logo.width)/2, (display_get_height() - icon_k_logo.height)/2, false);

  // Draw some text info
  char buffer[128];
  // snprintf(buffer, sizeof(buffer), "KITE'S CIRCUIT SWORD");
  snprintf(buffer, sizeof(buffer), "KITERETRO"); // 9 CHARS
  drawStringRGB(display_get_width()/2 - ((9 * 8) / 2), display_get_height()/2 + icon_k_logo.height/2 + 10, buffer, &colour_white, image);
  */

  IMAGE_T icon = display_load_raw_rgba(icon_k_logo_full.data,icon_k_logo_full.len, icon_k_logo_full.width, icon_k_logo_full.height, icon_k_logo_full.type);
  display_draw_image_on_image(image, &icon, (display_get_width() - icon_k_logo_full.width)/2, (display_get_height() - icon_k_logo_full.height)/2, false);

  // Update the canvas/image to screen
  changeSourceAndUpdateImageLayer(&base_layer);

  // Sleep
  usleep(3000000); //3s

  // Clear canvas
  clearImageRGB(image, &colour_clear);
  changeSourceAndUpdateImageLayer(&base_layer);
}

//-----------------------------------------------------------------------------

// Create an image layer
IMAGE_LAYER_T display_create_image_layer (uint32_t canvas_x, uint32_t canvas_y, uint32_t layer)
{
  fprintf(stdout, "[i] NEW CANVAS X[%i] Y[%i]\n", canvas_x, canvas_y);

  // Error check the values
  if (canvas_x > 1920 || canvas_y > 1920) {
    perror("Bad display size");
    exit(EXIT_FAILURE);
  }

  // Create the layer
  IMAGE_LAYER_T new_layer;
  initImageLayer(&new_layer, canvas_x, canvas_y, VC_IMAGE_RGBA16);
  createResourceImageLayer(&new_layer, layer);

  // Return it for use
  return new_layer;
}

//-----------------------------------------------------------------------------

// Add a canvas/layer pointer to display
void display_add_image_layer(IMAGE_LAYER_T layer, uint32_t offset_x, uint32_t offset_y)
{
  // Add it to the screen
  addElementImageLayerOffset(&layer, offset_x, offset_y, display, update);
}

//-----------------------------------------------------------------------------

// Configure the display number
void display_set_display(uint32_t disp)
{
  display_number = disp;
}

//-----------------------------------------------------------------------------

// Init the display
void display_init()
{
  printf("[i] display_init..\n");

  //Init BCM (PI)
  bcm_host_init();

  // Init display stuff
  display = vc_dispmanx_display_open(display_number);
  assert(display != 0);

  int result = vc_dispmanx_display_get_info(display, &info);
  assert(result == 0);

  update = vc_dispmanx_update_start(0);
  assert(update != 0);

  // Create some colours
  colour_black =    display_int2rgba(0,   0,   0,   255);
  colour_white =    display_int2rgba(255, 255, 255, 255);
  colour_red =      display_int2rgba(255, 0,   0,   255);
  colour_green =    display_int2rgba(0,   255, 0,   255);
  colour_blue =     display_int2rgba(0,   0,   255, 255);
  colour_clear =    display_int2rgba(0,   0,   0,   0  );
  colour_bg_light = display_int2rgba(30,  30,  30,  200);
  colour_bg_mlight= display_int2rgba(0,   0,   0,   200);
  colour_bg_vlight= display_int2rgba(255, 255, 255, 200);


  // Create base canvas
  base_layer = display_create_image_layer(display_get_width(), display_get_height(), 99999);

  // Apply to display
  display_add_image_layer(base_layer, display_get_start_x(), display_get_start_y());
}

// Finalise after all layers have been added
void display_init_finalise()
{
  printf("[i] display_init_finalise..\n");

  // Create screen update(r)
  assert(vc_dispmanx_update_submit_sync(update) == 0);

  // Start the first draw (splash screen)
  draw_splash();
}

//-----------------------------------------------------------------------------

// Destory the display
void display_unload()
{
  printf("[i] display_unload..\n");

  int result = vc_dispmanx_display_close(display);
  assert(result == 0);
}

//-----------------------------------------------------------------------------
