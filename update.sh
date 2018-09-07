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

# THIS MUST BE RUN FROM THE GIT CURRENT DIRECTORY (cd your way there)

if [ "$EUID" -ne 0 ]
  then echo "Please run as root (sudo)"
  exit 1
fi

# Stop HUD
echo "Stopping HUD.."
systemctl stop cs-osd.service > /dev/null  # Silience this as it may not exist
systemctl stop cs-hud.service

echo "Stopping CLONER.."
systemctl stop dpi-cloner.service

# Update git
echo "Updating HUD from git.."
git fetch --all
git reset --hard origin/master

# Set permissions on the initial required
chmod +x install.sh update.sh

# Perform re-install
echo "Performing script setup.."
./install.sh YES

# Start HUD
echo "Starting HUD.."
systemctl start cs-hud.service

# DONE
echo "DONE!"
