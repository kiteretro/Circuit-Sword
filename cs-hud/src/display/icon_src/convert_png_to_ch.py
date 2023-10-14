#!/usr/bin/env python
# sudo apt-get install python-serial

#
# This file originates from Kite's Circuit-Sword control board project.
# Author: Kite (Giles Burgess)
#
# THIS HEADER MUST REMAIN WITH THIS FILE AT ALL TIMES
#
# This firmware is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This firmware is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this repo. If not, see <http://www.gnu.org/licenses/>.
#

import time
import os, signal, sys
import subprocess
import re
import struct

root_dir = "/home/pi/Circuit-Sword-HUD/display"
png_dir = "%s/icon_src" % root_dir
converter_bin = "%s/png_converter/png_converter" % root_dir
c_output = "%s/icon.c" % root_dir
h_output = "%s/icon.h" % root_dir

c_header = """
#include "icon.h"
"""

c_footer = """
"""

h_header = """
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
"""

h_footer = """
//-----------------------------------------------------------------------------

#endif
"""

# End results
c_file = []
h_file = []
png_files = []

# Headers of files
for line in h_header.split(os.linesep):
  h_file.append(line)
for line in c_header.split(os.linesep):
  c_file.append(line)

# Get list of files
all_files = os.listdir(png_dir)
for f in sorted(all_files):
  if ".png" in f:
    png_files.append(f)

# For each matched file
for png in png_files:
  png_name = None
  png_path = "%s/%s" % (png_dir, png)
  print("[*] File: %s" % png)

  # Get file name
  res = re.match('(.*)\.png', png)
  if res:
    png_name = res.group(1).lower()
  else:
    raise Exception("Cannot get file name of %s" % png)

  print("    Name: %s" % png_name)
  print("    Path: %s" % png_path)

  # Execute conversion
  proc = subprocess.Popen([converter_bin, '-p', png_path, '-n', png_name], stdout = subprocess.PIPE)
  out, err = proc.communicate()

  # Extract data
  pos = 'UNKNOWN'
  for line in out.split(os.linesep):

    # Next line start header/source
    if pos == 'HEADER_TMP1':
      if "--" in line:
        pos = 'HEADER_TMP2'
      else:
        pos = 'UNKNOWN'
    elif pos == 'SOURCE_TMP1':
      if "--" in line:
        pos = 'SOURCE_TMP2'
      else:
        pos = 'UNKNOWN'
    elif pos == 'HEADER_TMP2':
        pos = 'HEADER'
    elif pos == 'SOURCE_TMP2':
        pos = 'SOURCE'

    # Header/source enable
    if "HEADER" in line:
      pos = 'HEADER_TMP1'
    elif "SOURCE" in line:
      pos = 'SOURCE_TMP1'

    # Actual header/source
    if pos == 'HEADER':
      if line:
        h_file.append(line)
    elif pos == 'SOURCE':
      c_file.append(line)

# Add footers
for line in h_footer.split(os.linesep):
  h_file.append(line)
for line in c_footer.split(os.linesep):
  c_file.append(line)

# Write to files
fh = open(h_output, 'w')
for line in h_file:
  fh.write("%s\n" % line)
fh.close()
print("[i] Saved .h file: %s" % h_output)

fh = open(c_output, 'w')
for line in c_file:
  fh.write("%s\n" % line)
fh.close()
print("[i] Saved .c file: %s" % c_output)

# done
