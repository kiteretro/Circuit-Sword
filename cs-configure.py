#!/usr/bin/env python

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
import serial
import subprocess
import re
import struct
import argparse
import logging
import logging.handlers

# PARSER LOGIC
parser = argparse.ArgumentParser(description='Circuit Sword Configuration')
parser.add_argument("-help", action="store_true", default=False, dest='help', help="show help message")
parser.add_argument("-debug", action="store_true", default=False, dest='debug', help="show debug messages")
parser.add_argument("--serport", action="store", default='/dev/ttyACM0', type=str, dest='serport', help="the serial port path")
args = parser.parse_args()

# Show help
if args.help:
  print("HELP")
  sys.exit(0)

#-------------------------------------------------------------------------------

# Setup
level = logging.INFO
if args.debug:
  level = logging.DEBUG
logging.basicConfig(level=level)
logging.info("Program Started")

#-------------------------------------------------------------------------------

# Mapping data
mapping = {
  's_mode': 0,
  's_wifi': 1,
  's_aud': 2,
  's_info': 3,
  's_avol': 4,
  's_dpad_joy': 5,
  's_dvol': 6,

  'j_iscalib1': 0,
  'j_iscalib2': 1,
  'j_xinvert1': 2,
  'j_yinvert1': 3,
  'j_xinvert2': 4,
  'j_yinvert2': 5,

  'b_up': 0,
  'b_down': 1,
  'b_left': 2,
  'b_right': 3,
  'b_a': 4,
  'b_b': 5,
  'b_x': 6,
  'b_y': 7,
  'b_start': 8,
  'b_select': 9,
  'b_l1': 10,
  'b_l2': 11,
  'b_r1': 12,
  'b_r2': 13,
  'b_c1': 14,
  'b_c2': 15,
}

#-------------------------------------------------------------------------------

# Check cs-hud not running (as it uses the serial port)
p = os.popen("service cs-hud status").readlines()
for line in p:
  if "active (running)" in line:
    logging.exception("ERROR: 'cs-hud' is still running, stop it first with 'sudo service cs-hud stop'")
    sys.exit(1)

# Set up a port
try:
  ser = serial.Serial(
    port=args.serport,
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=15
  )
except Exception as e:
  logging.exception("ERROR: Failed to open serial port")
  sys.exit(1)

#-------------------------------------------------------------------------------

# Serial read function
def readSerial(msg, length=1):
  ret = []

  running = True
  try:
    logging.debug("Command [%s].." % msg)
    ser.write(msg)

    while running:
      # Read something
      ret.append(ser.read())

      if len(ret) >= length:
        break;

      # Check for end byte(s)
      endcounter = 0
      for c in ret:
        if c == '?':
          logging.error("ERROR: Bad return for command [%s] - Not supported! Update the Arduino code.." % msg)
          endcounter = 2
        elif c == 'O':
          endcounter = 1
        elif endcounter == 1 and c == 'K':
          endcounter = 2
        else:
          endcounter = 0

      # Check if end of bytes
      if endcounter == 2:
        break
      
  except Exception as err:
    logging.error("ERROR: Serial get failed! %s" % err)
    return None

  logging.debug("Command [%s] returned: %s" % (msg, ret))
  return ret

# Convert unicode to fake byte
def convert_to_fake_byte(data):
  return bin(convert_to_byte(data))[2:].rjust(8, '0')[::-1]
def convert_to_byte(data):
  return ord(data)
def convert_to_decimal(data):
  decimal = 0
  pos = 1
  for byte in data:
    for bit in convert_to_fake_byte(byte):
      if bit == '1':
        decimal += pos
      pos *= 2
  return decimal

# Get data
def convert_data(config, command, is_binary=True, length=1):
  data = readSerial(command, length=length)

  # Binary config data convert
  if is_binary:
    if length > 1:
      raise Exception("Not implemented")

    length = length * 8
    data = convert_to_fake_byte(data[0])

    if len(data) != length:
      raise Exception("ERROR: Bad data")

    for pos in range(0, length):
       for r in config:
         if mapping[r] == pos:
           config[r] = data[pos]

  # Value is an int
  else:
    return convert_to_decimal(data)

  return config

#-------------------------------------------------------------------------------

def get_status():
  return convert_data({
      's_mode': None,
      's_wifi': None,
      's_aud': None,
      's_info': None,
      's_avol': None,
      's_dpad_joy': None,
      's_dvol': None,
    },
    's'
  )
def set_info(state):
  # I 0/1
  return
def set_wifi(state):
  # W 0/1
  return
def set_aud(state):
  # A 0/1
  return
def toggle_dpad_joy():
  readSerial('d', length=2)
  return

def get_voltage():
  return convert_data({}, 'c', is_binary=False, length=2)

def get_volume():
  return convert_data({}, 'e', is_binary=False)
def set_volume(volume):
  # E + B0-100
  return
def get_avol_adc():
  return convert_data({}, 't', is_binary=False, length=2)
def toggle_avol():
  readSerial('C', length=2)
  return
def toggle_dvol():
  readSerial('z', length=2)
  return

def get_backlight():
  return convert_data({}, 'q', is_binary=False)
def set_backlight(backlight):
  # Q + B0-100
  return

def get_joystick_adc():
  return {
    'joy1_x': convert_data({}, 'u', is_binary=False, length=2),
    'joy1_y': convert_data({}, 'o', is_binary=False, length=2),
    'joy2_x': convert_data({}, 'p', is_binary=False, length=2),
    'joy2_y': convert_data({}, 'y', is_binary=False, length=2),
  }

def get_joystick_config():
  return convert_data({
      'j_iscalib1': None,
      'j_iscalib2': None,
      'j_xinvert1': None,
      'j_yinvert1': None,
      'j_xinvert2': None,
      'j_yinvert2': None,
    },
    'j'
  )
def set_joystick_config(toggle_j1=False, toggle_j2=False, invert_j1x=False, invert_j1y=False, invert_j2x=False, invert_j2y=False):
  if toggle_j1:
    readSerial('{', length=2)
  if toggle_j2:
    readSerial('}', length=2)
  if invert_j1x:
    readSerial('(', length=2)
  if invert_j1y:
    readSerial(')', length=2)
  if invert_j2x:
    readSerial('[', length=2)
  if invert_j2y:
    readSerial(']', length=2)
  return
def calibrate_joystick():
  readSerial('J', length=2)
  return

def get_button_state():
  return convert_data({
      'b_up': None,
      'b_down': None,
      'b_left': None,
      'b_right': None,
      'b_a': None,
      'b_b': None,
      'b_x': None,
      'b_y': None,
      'b_start': None,
      'b_select': None,
      'b_l1': None,
      'b_l2': None,
      'b_r1': None,
      'b_r2': None,
      'b_c1': None,
      'b_c2': None,
    },
    'B',
    length=2,
  )
#-------------------------------------------------------------------------------

def process_menu():
  c = {
    'status': get_status(),
    'joystick_config': get_joystick_config(),
    'joystick_status': get_joystick_adc(),
    'voltage_adc': get_voltage(),
    'volume': get_volume(),
    'volume_adc': get_avol_adc(),
    'backlight': get_backlight(),
  }

  # Show the config
  print("\nGENERAL INFORMATION")
  print("Mode pressed: %s" % c['status']['s_mode'])
  print("Wifi enabled: %s" % c['status']['s_wifi'])
  print("Backlight:    %s%%" % c['backlight'])

  print("\nVOLUME INFORMATION")
  print("Amp enabled:            %s" % c['status']['s_aud'])
  print("Current volume:         %s%%" % c['volume'])
  print("Digital rocker enabled: %s" % c['status']['s_dvol'])
  print("Analog volume enabled:  %s" % c['status']['s_avol'])
  print("Analog volume adc:      %s" % c['volume_adc'])

  print("\nJOYSTICK INFORMATION")
  print("JOY 1 enabled: %s - (X: %s Y:%s)" % (
    c['joystick_config']['j_iscalib1'],
    1024 - c['joystick_status']['joy1_x'] if c['joystick_config']['j_xinvert1'] == '1' else c['joystick_status']['joy1_x'],
    1024 - c['joystick_status']['joy1_y'] if c['joystick_config']['j_yinvert1'] == '1' else c['joystick_status']['joy1_y']))
  print("  JOY 1 X invert: %s" % c['joystick_config']['j_xinvert1'])
  print("  JOY 1 Y invert: %s" % c['joystick_config']['j_yinvert1'])
  print("JOY 2 enabled: %s - (X: %s Y:%s)" % (
    c['joystick_config']['j_iscalib2'],
    1024 - c['joystick_status']['joy2_x'] if c['joystick_config']['j_xinvert2'] == '1' else c['joystick_status']['joy2_x'],
    1024 - c['joystick_status']['joy2_y'] if c['joystick_config']['j_yinvert2'] == '1' else c['joystick_status']['joy2_y']))
  print("  JOY 2 X invert: %s" % c['joystick_config']['j_xinvert2'])
  print("  JOY 2 Y invert: %s" % c['joystick_config']['j_yinvert2'])

  print("""
MAIN MENU
---------
1 - Joystick calibration
2 - Invert JOY 1 X config
3 - Invert JOY 1 Y config
4 - Invert JOY 2 X config
5 - Invert JOY 2 Y config
6 - Toggle JOY 1 enabled config
7 - Toggle JOY 2 enabled config
8 - Toggle Analog Volume enabled config
9 - Toggle Digital Volume Rocker enabled config
ENTER - Refresh information
X - Quit

Enter selection followed by ENTER: """)
  # key = raw_input()
  key = sys.stdin.read(1)

  if key == '1':
    print(">>> Calibrating joystick..")
    print("SLOWLY ROTATE ALL THE JOYSTICKS IN A CIRCULAR MOTION THAT YOU WISH TO CALIBRATE NOW.\nTHIS WILL TAKE 10 SECONDS.\nCalibrating..")
    calibrate_joystick()
    print("Calibration complete!")
  elif key == '2':
    print(">>> Inverting JOY 1 X config..")
    set_joystick_config(invert_j1x=True)
  elif key == '3':
    print(">>> Inverting JOY 1 Y config..")
    set_joystick_config(invert_j1y=True)
  elif key == '4':
    print(">>> Inverting JOY 2 X config..")
    set_joystick_config(invert_j2x=True)
  elif key == '5':
    print(">>> Inverting JOY 2 Y config..")
    set_joystick_config(invert_j2y=True)
  elif key == '6':
    print(">>> Inverting JOY 1 enabled config..")
    set_joystick_config(toggle_j1=True)
  elif key == '7':
    print(">>> Inverting JOY 2 enabled config..")
    set_joystick_config(toggle_j2=True)
  elif key == '8':
    print(">>> Inverting ANALOG VOLUME enabled config..")
    toggle_avol()
  elif key == '9':
    print(">>> Inverting DIGITAL VOLUME ROCKER enabled config..")
    toggle_dvol()
  elif key == 'x' or key == 'X':
    sys.exit(0)
  else:
    print(">>> Refreshing..")
    return

  time.sleep(2)

# Main loop
try:
  while(True):
    process_menu()
except KeyboardInterrupt:
  pass
