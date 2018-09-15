#!/bin/bash

# 
# This file originates from Kite's Circuit Sword control board project.
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

HDMI_SWITCHER_PATH="/home/pi/Circuit-Sword/settings/reboot_to_hdmi.py"
CMD="sudo /usr/bin/python $HDMI_SWITCHER_PATH --hdmi --reboot"

# Check script exists first
if [ -f $HDMI_SWITCHER_PATH ]; then
  echo "[i] Main switcher script exists"
else
  echo
  echo "====================================="
  echo "[!] ERROR: The switcher file '$HDMI_SWITCHER_PATH' is missing!!"
  echo "====================================="
  sleep 3
  exit 1
fi

# Execute it!
eval "$CMD"
ret=$?

if [ $ret != 0 ] ; then
  echo
  echo "====================================="
  echo "[!] ERROR: Command exited with '$ret'"
  echo "====================================="
  sleep 3
  exit 1
fi
