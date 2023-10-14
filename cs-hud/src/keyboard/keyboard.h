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

#ifndef KEYBOARD_H
#define KEYBOARD_H

//-----------------------------------------------------------------------------

#include <stdio.h>

#include </usr/include/linux/input.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/signalfd.h>
#include <sys/inotify.h>

//-----------------------------------------------------------------------------
// STRUCTS

typedef struct {
  char *name;
  int   value;
} KB_KEY_T;

typedef struct {
  char *name;
  int   value;
  uint32_t mask;
  bool pressed;
} KB_CS_KEY_T;

typedef struct {
  char *name;
  int   value;
} dict;

//-----------------------------------------------------------------------------
// STATIC VARIABLES

//-----------------------------------------------------------------------------
// METHODS
void keyboard_init(void);
void keyboard_unload(void);

void keyboard_press(int id);
void keyboard_release(int id);
void keyboard_button(int id);

//-----------------------------------------------------------------------------

#endif
