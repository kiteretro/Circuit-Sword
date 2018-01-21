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

if [ "$EUID" -ne 0 ]
  then echo "Please run as root (sudo)"
  exit 1
fi

PWD=`pwd`

# Stop OSD
echo "Stopping OSD.."
systemctl stop cs-osd.service

echo "Stopping CLONER.."
systemctl stop dpi-cloner.service

# Update git
echo "Updating OSD from git.."
git fetch --all
git reset --hard origin/master

# Perform re-install
echo "Performing script setup.."
sh ./install.sh

# Start OSD
echo "Starting OSD.."
systemctl start cs-osd.service

# DONE
echo "DONE!"
