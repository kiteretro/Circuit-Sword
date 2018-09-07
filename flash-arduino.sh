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

COM="/dev/ttyACM0"
AVRDUDE="avrdude"
HEX="kite-arduino/CS_FIRMWARE/CS_FIRMWARE.ino.with_bootloader.leonardo.hex"

if [ "$EUID" -ne 0 ]
  then echo "Please run as root (sudo)"
  exit 1
fi

#####################################################################
# Functions
execute() { #STRING
  if [ $# != 1 ] ; then
    echo "ERROR: No args passed"
    exit 1
  fi
  cmd=$1
  
  echo "[*] EXECUTE: [$cmd]"
  eval "$cmd"
  ret=$?
  
  if [ $ret != 0 ] ; then
    echo "ERROR: Command exited with [$ret]"
    exit 1
  fi
  
  return 0
}

exists() { #FILE
  if [ $# != 1 ] ; then
    echo "ERROR: No args passed"
    exit 1
  fi
  
  file=$1
  
  if [ -f $file ]; then
    echo "[i] FILE: [$file] exists."
    return 0
  else
    echo "[i] FILE: [$file] does not exist."
    return 1
  fi
}

existsdev() { #file
  if [ $# != 1 ] ; then
    echo "ERROR: No args passed"
    exit 1
  fi
  
  file=$1
  
  if [ $(ls $file | wc -l) == 1 ] ; then
    echo "[i] FILE: [$file] exists."
    return 0
  else
    echo "[i] FILE: [$file] does not exist."
    return 1
  fi
}

installed() { #BINARY
  if [ $# != 1 ] ; then
    echo "ERROR: No args passed"
    exit 1
  fi
  
  file=$1
  
  if [ $(which $file | wc -l) == 1 ] ; then
    echo "[i] BIN: [$file] exists."
    return 0
  else
    return 1
    echo "[i] BIN: [$file] does not exist."
  fi
}

#####################################################################

# Check hex file exists
if ! exists "$HEX" ; then
  echo "ERROR: HEXFILE [$HEX] does not exist"
  exit 1
fi

# Check COM exists
if ! existsdev "$COM" ; then
  echo "ERROR: COM PORT [$COM] does not exist"
  exit 1
fi

# Check avrdude exists
if ! installed "$AVRDUDE" ; then
  echo "ERROR: $AVRDUDE does not exist in path
Try installing with: sudo apt-get install avrdude"
  exit 1
fi

read -p "This will flash the arduino with the latest firmware, it MIGHT fail, are you sure?
If you are in doubt, flash using a PC and Arduino software.
[Y/y to continue, N/n cancel]: " -n 1 -r

echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
  for i in {10..01} ; do
    echo "$i second(s) until flashing.. CTRL + C to cancel now!"
    sleep 1
  done
else
  echo "Cancelling.."
  exit 0
fi

# Stop HUD
echo "Stopping HUD.."
systemctl stop cs-osd.service > /dev/null
systemctl stop cs-hud.service
sleep 2

# Reset the AVR
echo "Resetting Arduino.."
python - <<END
import serial
import time
import os

serport = "$COM"

ser = serial.Serial(serport, 57600)
ser.close()

ser.baudrate = 1200
ser.open()
ser.setRTS(True)
ser.setDTR(False)
ser.close()

while not os.path.exists(serport):
  print("Waiting for reset..")
  time.sleep(1)

print("Reset completed")
END

sleep 2

# Flash
echo "Flashing Arduino.."
avrdude -P $COM -c avr109 -p atmega32u4 -b 57600 -U flash:w:$HEX:i
ret=$?
if [ $ret != 0 ]; then
  echo "ERROR: Failed to flash Arduino [$ret]
At this point you may be lucky and in 10 seconds the Arduino will reboot
Othrewise you can try again, or try plugging in to a PC and using the Arduino software to upload
Failing that, you will need to program using an AVRISP"
  exit $ret
fi

# Sleep a little
echo "Sleeping for 10 seconds for Arduino to come back up.."
sleep 10

# Done check
if existsdev "/dev/ttyACM*" ; then
  echo "SUCCESS!"
else
  echo "UNKNOWN: COM port did not come back, please reboot"
fi

# DONE
echo "DONE!"

echo
echo "## PLEASE REBOOT FOR ANY CHANGES TO TAKE EFFECT ##"
echo "##    THE HUD WILL NOT WORK WITHOUT A REBOOT    ##"
echo
