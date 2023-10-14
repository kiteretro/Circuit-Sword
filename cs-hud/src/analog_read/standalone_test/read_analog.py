#!/usr/bin/env python

#
# This file originates from Kite's AIO control board project.
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

#
# Read an ANALOG pin with a raspberry pi GPIO
#
# PIN --- 51k --- VBATT
#      |
#      | ---- 100k --- GND
#      |
#      | ---- 1uF  --- GND
#

import RPi.GPIO as GPIO, time
import math

# Anlog config values
vc = 2.0  # Switching voltage (v)     (leave this)
r = 31.0  # RC resistance     (k ohm) (adjust this!)
c = 1.0   # RC capacitance    (uF)    (leave this)
vpin = 26 # Pi pin

# Broadcom GPIO references
GPIO.setmode(GPIO.BCM)

# READ ANALOG
def analog_read_start(pin):
  GPIO.setup(pin, GPIO.OUT)
  GPIO.output(pin, GPIO.LOW)
  time.sleep(0.1)

  analog_start_time = time.time()
  GPIO.setup(pin, GPIO.IN)
  res = GPIO.wait_for_edge(pin, GPIO.RISING, timeout=500)
  analog_end_time = time.time()
  GPIO.setup(pin, GPIO.OUT)
  GPIO.output(pin, GPIO.LOW)

  print("%f", (analog_end_time - analog_start_time))

  voltage = - ( vc / (math.exp(-((analog_end_time - analog_start_time)*1000)/(r*c))-1) )

  if res is None:
    return False
  else:
    return voltage

print("FIRST READ TEST")
analog_read_start(vpin)
print("FIRST READ DONE")

# Main program loop
while True:
  v = analog_read_start(vpin)
  if v:
    print("VOLTAGE: %.2fv" % v)
  else:
    print("NO VOLTAGE FOUND")

  time.sleep(1)
