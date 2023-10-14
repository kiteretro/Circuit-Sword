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

#ifndef SERIALPORT_H
#define SERIALPORT_H

//-------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <sys/ioctl.h>

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

bool serial_init(const char* serport);
bool serial_send(char* tx_buffer, uint8_t len);
int serial_receive(char* rx_buffer, uint8_t lenmax);
int serial_receive_bytes(char *rx_buffer, uint8_t len, uint16_t timeout);
void serial_clear();
void serial_unload();
bool serial_opened();
uint8_t serial_available();

//-------------------------------------------------------------------------

#endif
