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

# This file exists in '/opt/retropie/configs/all/autostart.sh'

# Restart to hdmi on boot to fix any changes to file
sudo /usr/bin/python /home/pi/Circuit-Sword/settings/reboot_to_hdmi.py --check

# Load config file and action
CONFIGFILE="/boot/config-cs.txt"
if [ -f $CONFIGFILE ]; then
  
  source $CONFIGFILE
  
  if [[ -n "$STARTUPEXEC" ]] ; then
    echo "Starting STARTUPEXEC.."
    $STARTUPEXEC &
  fi
  
  if [[ "$CLONER" == "ON" ]] ; then
    if [[ $(tvservice -s | grep LCD) ]] ; then
      echo "Starting CLONER.."
      sudo systemctl start dpi-cloner.service
    fi
  fi
  
  if [[ "$MODE" == "TESTER" && -n "$TESTER" ]] ; then
    echo "Starting TESTER.."
    python $TESTER
  elif [ "$MODE" == "SHELL" ] ; then
    echo "Starting SHELL.."
    exit 0
  else
    echo "Starting EMULATIONSTATION.."
    emulationstation #auto
  fi
  
else
  
  echo "Starting EMULATIONSTATION.."
  emulationstation #auto
  
fi
