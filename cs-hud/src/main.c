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

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <inttypes.h>
#include <libgen.h>

#include "bcm_host.h"

#include "defs.h"
#include "config.h"
#include "manager.h"
// #include "display_manager.h"

//----------------------------------------------------------------------------

volatile bool run = true;

//----------------------------------------------------------------------------

// Function to handle external interrupt
static void signalHandler(int signalNumber)
{
  switch (signalNumber)
  {
  case SIGUSR1:
    break;
  case SIGINT:
  case SIGTERM:
    run = false;
    break;
  };
}

//----------------------------------------------------------------------------

// MAIN
int main(int argc, char *argv[])
{
  setbuf(stdout, NULL);
  printf("Kite's Circuit Sword HUD\n");

  //-------------------------------------------------------------------
  // Special configs (this should really be improved)
  bool opt_hide_batt = 0;

  //-------------------------------------------------------------------
  // Read command line options
  int opt = 0;
  while ((opt = getopt(argc, argv, "hs")) != -1) {
    switch (opt) {

      case 's':
        opt_hide_batt = 1;
        break;

      case 'h':
      default:
        fprintf(stderr, "Usage: %s [-s]\n", basename(argv[0]));
        fprintf(stderr, "    -s  Hide the battery icon (unless low)\n");
        exit(EXIT_FAILURE);
        break;
    }
  }

  //-------------------------------------------------------------------
  // Register signal handlers
  if (signal(SIGINT, signalHandler) == SIG_ERR)
  {
    perror("installing SIGINT signal handler");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGTERM, signalHandler) == SIG_ERR)
  {
    perror("installing SIGTERM signal handler");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGUSR1, signalHandler) == SIG_ERR)
  {
    perror("installing SIGUSR1 signal handler");
    exit(EXIT_FAILURE);
  }

  //-------------------------------------------------------------------
  // Inits
  config_init();
  manager_init();

  //-------------------------------------------------------------------
  // Special configs (this should really be improved)
  if (opt_hide_batt) {
    c.hide_battery = 1;
  }

  //-------------------------------------------------------------------
  // Main running loop
  while (run == true) {
    manager_process();
    usleep(16000); //16ms
  }

  //-------------------------------------------------------------------
  // TODO make a handler for this exit
  manager_unload();
  config_unload();

  return 0;
}
