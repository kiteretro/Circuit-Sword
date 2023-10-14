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

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

//-----------------------------------------------------------------------------

#include <stdio.h>

#include "display.h"
#include "state.h"
#include "defs.h"
#include "keyboard.h"

//-----------------------------------------------------------------------------

#define REFRESH_SPEED_SLOW 5
#define REFRESH_SPEED_FAST 1

#define ICON_HOLD_COUNT    1
#define ICON_START_COUNT   10

#define KB_KBS   2
#define KB_LOWER 0
#define KB_UPPER 1

#define KEYBOARD_KEY_ROWS 13
#define KEYBOARD_KEY_COLS 5

#define KEYBOARD_WIDTH 222
#define KEYBOARD_HEIGHT 86

#define KEYBOARD_X_OFFSET 0
#define KEYBOARD_Y_OFFSET 16
#define KEYBOARD_KEY_WIDTH 16
#define KEYBOARD_KEY_HEIGHT 16
#define KEYBOARD_KEY_X_OFFSET 1
#define KEYBOARD_KEY_Y_OFFSET 1

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

void display_manager_process(void);
void display_manager_init(void);
void display_manager_unload(void);
void display_manager_clear(void);

//-----------------------------------------------------------------------------

#endif
