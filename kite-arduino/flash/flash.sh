# FLASH ATMEGA32u4 WITH HEX
#
# Author: Giles Burgess (Kite/Geebles)
# GitHub: https://github.com/geebles/Circuit-Sword
#
# reset.py from https://github.com/nicholaskell/Arduino_Loader
#
# Run with ./flash.sh hexfile.hex
#

# Arguments check
if [ "$#" -ne 1 ]; then
  echo "Usage: ./flash.sh <hexfile>"
  exit 1
fi

# Config
HEX=$1
PORT=/dev/ttyACM0

# Port existence check
if [ ! -e $PORT ]; then
  echo "ERROR: Com port does not exist"
  exit 1
fi

# Hex existence check
if [ ! -e $HEX ]; then
  echo "ERROR: Hex file doesn't exist"
  exit 1
fi

# AVRDUDE existence check
if [ ! -e `which avrdude` ]; then
  echo "ERROR: avrdude does not exist in PATH"
  exit 1
fi

# Enter board into bootloader mode
python reset.py $PORT
ret=$?
if [ $ret != 0 ]; then
  printf "Error while trying to reset MCU [$ret]"
  exit $ret
fi

# Sleep a little bit
sleep 1

# Flash the hex!
avrdude -P $PORT -c avr109 -p atmega32u4 -b 57600 -U flash:w:$HEX:i
ret=$?
if [ $ret != 0 ]; then
  printf "Error while trying to flash MCU [$ret]"
  exit $ret
fi
