/*
 * giles test display
 */

#include <stdio.h>
#include "display_tools.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <inttypes.h>
#include <stdint.h>
#include <libgen.h>

//-----------------------------------------------------------------------------

char* bname;
void help()
{
  fprintf(stderr, "Usage: %s -p <file> -n <name>\n", bname);
  fprintf(stderr, "    -p <file> : png data file path\n");
  fprintf(stderr, "    -n <str>  : name of output variable\n");
  exit(EXIT_FAILURE);
}

// MAIN
int main(int argc, char *argv[])
{
  printf("Kite's PNG to .c/.h Converter\n");

  //-------------------------------------------------------------------

  int opt = 0;
  char* png_file = "";
  char* png_name = "";
  bname = basename(argv[0]);

  //-------------------------------------------------------------------
  // Read command line options
  while ((opt = getopt(argc, argv, "p:n:")) != -1)
  {
    switch (opt)
    {

      case 'p':

        png_file = strdup(optarg);
        break;

      case 'n':

        png_name = strdup(optarg);
        break;

      default:

        help();
        break;
    }
  }

  // Missing arg check
  if (strcmp("", png_file) == 0 || strcmp("", png_name) == 0)
  {
    help();
  }

  //-------------------------------------------------------------------

  IMAGE_T img = display_load_png(png_file);
  display_dump_img_to_c(&img, png_name);

  return 0;
}
