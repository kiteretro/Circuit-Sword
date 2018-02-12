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

import RPi.GPIO as GPIO
import time
import os, signal, sys
import serial
import subprocess
import re
import struct
import logging
import logging.handlers

try:
    from configparser import ConfigParser
except ImportError:
    from ConfigParser import ConfigParser  # ver. < 3.0

# Config variables
root_dir = '/home/pi/Circuit-Sword/'
bin_dir = root_dir + 'cs-osd/'
ram_dir = '/ramdisk/'
ini_data_file = ram_dir + 'cs-osd-data.ini' if os.path.exists(ram_dir) else root_dir + 'cs-osd/data.ini'
ini_config_file = bin_dir + 'cs-osd/config.ini'
osd_path = bin_dir + 'cs-osd/cs-osd'
rfkill_path = root_dir + 'rfkill/rfkill'
config_file = '/boot/config-cs.txt'

# Hardware variables
pi_wifien = 34
pi_overtemp = 35
pi_chrg = 36
pi_shdn = 37
pi_pgin = 38
serport = '/dev/ttyACM0'

# Software variables
settings_shutdown = 1  # Enable ability to shut down system
tick_max = 8

# Setup
logging.basicConfig(level=logging.DEBUG)
logging.info("Program Started")

# Init GPIO pins
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(pi_chrg, GPIO.IN)
GPIO.setup(pi_pgin, GPIO.IN)
GPIO.setup(pi_shdn, GPIO.IN)
GPIO.setup(pi_overtemp, GPIO.OUT)
GPIO.output(pi_overtemp, GPIO.HIGH)
GPIO.setup(pi_wifien, GPIO.OUT)
GPIO.output(pi_wifien, GPIO.HIGH)

# Batt variables
voltscale = 203.5  # ADJUST THIS
currscale = 640.0
resdivmul = 4.0
resdivval = 1000.0
dacres = 33.0
dacmax = 1023.0

batt_threshold = 4
batt_low = 330
batt_shdn = 320
batt_islow = False

temperature_max = 60.0
temperature_threshold = 5.0
temperature_isover = False

# Wifi variables
wifi_state = 'UNKNOWN'
wifi_off = 0
wifi_warning = 1
wifi_error = 2
wifi_1bar = 3
wifi_2bar = 4
wifi_3bar = 5

# Volume variables
vol_last = 0

# Set up a port
try:
    ser = serial.Serial(
        port=serport,
        baudrate=115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=0.3
    )
except Exception as e:
    logging.exception("ERROR: Failed to open serial port")
    sys.exit(1)

# Set up configOSD file
configOSD = ConfigParser()
configOSD.add_section('protocol')
configOSD.set('protocol', 'version', 1)
configOSD.add_section('data')
configOSD.set('data', 'voltage', '-.--')
configOSD.set('data', 'temperature', '--.-')
configOSD.set('data', 'showdebug', 1)
configOSD.set('data', 'showwifi', 0)
configOSD.set('data', 'showmute', 0)

try:
    with open(ini_data_file, 'w') as configfile:
        configOSD.write(configfile)
except Exception as e:
    logging.exception("ERROR: Failed to create configOSD file")
    sys.exit(1)

# Set up OSD service
try:
    osd_proc = subprocess.Popen([osd_path, "-d", ini_data_file, "-c", ini_config_file])
    time.sleep(1)
    osd_poll = osd_proc.poll()
    if (osd_poll):
        logging.error("ERROR: Failed to start OSD, got return code [" + str(osd_poll) + "]\n")
        sys.exit(1)
except Exception as e:
    logging.exception("ERROR: Failed start OSD binary")
    sys.exit(1)


# Class to fake a section header for configparser
class FakeSecHead(object):
    def __init__(self, fp):
        self.fp = fp
        self.sechead = '[main]\n'

    def readline(self):
        if self.sechead:
            try:
                return self.sechead
            finally:
                self.sechead = None
        else:
            return self.fp.readline()


# Set up a settings config file
configMAIN = ConfigParser()
if (os.path.isfile(config_file)):
    try:
        configMAIN = ConfigParser()
        configMAIN.readfp(FakeSecHead(open(config_file)))

        # Analyse values
        if (configMAIN.get('main', 'mode') == "TESTER"):
            settings_shutdown = 0

    except Exception as e:
        logging.exception("ERROR: could not load configMAIN file")
else:
    logging.warning("WARNING: Failed to find configMAIN file")


# Serial read function
def readSerial(msg):
    ret = None
    attempt = 0
    attempts = 3

    while attempt != attempts:
        try:
            ser.write(msg)
            ret = int(ser.readline().rstrip('\r\n'))
            attempt = attempts
        except Exception, err:
            logging.error("ERROR: Serial get failed! ", err)
            attempt += 1

    return ret


# Check for shutdown state
def checkShdn():
    state = not GPIO.input(pi_shdn)
    if (state):
        logging.info("SHUTDOWN")
        doShutdown()


# Check the charging state
def checkCharging():
    state = 0
    chrgstate = GPIO.input(pi_chrg)
    pgstate = GPIO.input(pi_pgin)
    if (chrgstate):
        state = 1
    elif (pgstate):
        state = 2
    logging.info("CHRG [" + str(chrgstate) + "] PGIN [" + str(pgstate) + "] STATE [" + str(state) + "]")
    return state


# Check and apply volume
def checkVolume():
    global vol_last
    vol = readVol()

    if vol is not None:
        if vol != vol_last:
            vol_last = vol
            os.system("amixer sset PCM %s%%" % vol)


# Read voltage
def readVoltage():
    voltVal = readSerial('V')
    if voltVal is not None:
        voltVal = int(voltVal)
        volt = int(((voltVal * voltscale * dacres + (dacmax * 5)) / ((dacres * resdivval) / resdivmul)))

        logging.info("VoltVal [" + str(voltVal) + "]")
        logging.info("Volt    [" + str(volt) + "]V")

        global batt_islow

        if (batt_islow):
            if (volt > batt_low + batt_threshold):
                batt_islow = False
                logging.info("BATT OK")
            if (volt < batt_shdn):
                logging.info("VERY LOW BATT")
                # doShutdown()

        else:
            if (volt < batt_low):
                batt_islow = True
                logging.info("LOW BATT")

        return volt
    else:
        return 350


# Get voltage percent
def getVoltagepercent(volt):
    return clamp(int(float(volt - batt_shdn) / float(420 - batt_shdn) * 100), 0, 100)


# Read current
def readCurrent():
    currVal = readSerial('C')
    if currVal is not None:
        currVal = int(currVal)
        curr = int((currVal * (dacres / (dacmax * 10)) * currscale))

        logging.info("CurrVal [" + str(currVal) + "]")
        logging.info("Curr    [" + str(curr) + "]mA")
        return curr
    else:
        return 0


# Read mode
def readModeDebug():
    debugVal = readSerial('i')
    if debugVal is not None:
        debugVal = int(debugVal)
        logging.info("Info    [" + str(debugVal) + "]")
        return debugVal
    else:
        return 0


# Read wifi
def readModeWifi():
    ret = wifi_off

    wifiVal = readSerial('w')
    if wifiVal is not None:
        wifiVal = int(wifiVal)

        logging.info("Wifi    [" + str(wifiVal) + "]")

        global wifi_state

        if (wifiVal):
            if (wifi_state != 'ON'):
                wifi_state = 'ON'
                logging.info("Wifi    [ENABLING]")
                GPIO.output(pi_wifien, GPIO.HIGH)
                try:
                    out = subprocess.check_output(['sudo', rfkill_path, 'unblock', 'wifi'])
                    logging.info("Wifi    [" + str(out) + "]")
                    out = subprocess.check_output(['sudo', rfkill_path, 'unblock', 'bluetooth'])
                    logging.info("BT      [" + str(out) + "]")
                except Exception, err:
                    logging.info("Wifi    : ", err)
                ret = wifi_warning
            else:
                # Get signal strength
                raw = subprocess.check_output(['cat', '/proc/net/wireless'])
                strengthObj = re.search(r'.wlan0: \d*\s*(\d*)\.\s*[-]?(\d*)\.', raw, re.I)
                if strengthObj:
                    strength = 0
                    if (int(strengthObj.group(1)) > 0):
                        strength = int(strengthObj.group(1))
                    elif (int(strengthObj.group(2)) > 0):
                        strength = int(strengthObj.group(2))
                    logging.info("Wifi    [" + str(strength) + "]strength")
                    if (strength > 55):
                        ret = wifi_3bar
                    elif (strength > 40):
                        ret = wifi_2bar
                    elif (strength > 5):
                        ret = wifi_1bar
                    else:
                        ret = wifi_warning
                else:
                    logging.info("Wifi    [---]strength")
                    ret = wifi_error
        else:
            if (wifi_state != 'OFF'):
                wifi_state = 'OFF'
                logging.info("Wifi    [DISABLING]")
                GPIO.output(pi_wifien, GPIO.LOW)
                try:
                    out = subprocess.check_output(['sudo', rfkill_path, 'block', 'wifi'])
                    logging.info("Wifi    [" + str(out) + "]")
                    out = subprocess.check_output(['sudo', rfkill_path, 'block', 'bluetooth'])
                    logging.info("BT      [" + str(out) + "]")
                except Exception, e:
                    logging.info("Wifi    : " + str(e.output))
                ret = wifi_error

        return ret
    else:
        return wifi_warning


# Read mute
def readModeMute():
    audVal = readSerial('a')
    if audVal is not None:
        audVal = int(audVal)

        logging.info("Audio   [" + str(audVal) + "]")
        if (audVal):
            return 0
        else:
            return 1
    else:
        return 0


# Read vol from board
def readVol():
    volVal = readSerial('f')
    if volVal is not None:
        volVal = int(volVal)

        logging.info("Volume  [" + str(volVal) + "]")
        return volVal
    else:
        return None


# Read CPU temp
def getCPUtemperature():
    res = os.popen('vcgencmd measure_temp').readline()
    return float(res.replace("temp=", "").replace("'C\n", ""))


# Check temp
def checkTemperature():
    temp = getCPUtemperature()

    global temperature_isover

    if (temperature_isover):
        if (temp < temperature_max - temperature_threshold):
            temperature_isover = False
            GPIO.output(pi_overtemp, GPIO.HIGH)
            logging.info("TEMP OK")
    else:
        if (temp > temperature_max):
            temperature_isover = True
            GPIO.output(pi_overtemp, GPIO.LOW)
            logging.info("OVERTEMP")
    return temp


# Do a shutdown
def doShutdown():
    os.system("sudo shutdown -h now")
    try:
        sys.stdout.close()
    except:
        pass
    try:
        sys.stderr.close()
    except:
        pass
    sys.exit(0)


# Create ini configOSD
def createINI(volt, chrg, curr, temp, debug, wifi, mute, file):
    # configOSD.set('data', 'voltage', '{0:.2f}'.format(volt/100.00))
    configOSD.set('data', 'voltage', volt)
    configOSD.set('data', 'chargestate', chrg)
    configOSD.set('data', 'temperature', temp)
    configOSD.set('data', 'showdebug', debug)
    configOSD.set('data', 'showwifi', wifi)
    configOSD.set('data', 'showmute', mute)

    with open(ini_data_file, 'w') as configfile:
        configOSD.write(configfile)

    osd_proc.send_signal(signal.SIGUSR1)


# Show MP4 overlay
def doVidOverlay(overlay):
    os.system("/usr/bin/omxplayer --no-osd --layer 999999 " + overlay + " --alpha 160;")


# Show PNG overlay
def doPngOverlay(overlay):
    try:
        os.system("kill -s 9 `pidof pngview`")
    except:
        pass
    try:
        os.system("./pngview -b 0 -l 999999 " + overlay + "&")
    except:
        pass


# Misc functions
def clamp(n, minn, maxn):
    return max(min(maxn, n), minn)


# Main loop
try:
    print "STARTED!"
    tick = 0
    while 1:

        try:
            # Check for shutdown and action it
            if (settings_shutdown):
                checkShdn()

            # Check and apply volume
            checkVolume()

            # Increment loop counter
            tick += 1

            # When loop at max, update the OSD
            if tick == tick_max:

                volt = readVoltage()
                chrg = checkCharging()
                temp = checkTemperature()
                debug = readModeDebug()
                wifi = readModeWifi()
                mute = readModeMute()

                createINI(volt, chrg, 0, temp, debug, wifi, mute, ini_data_file)
                tick = 0

            # Otherwise sleep a little
            else:
                time.sleep(0.1)
        except Exception, err:
            logging.error("ERROR: ", err)
            time.sleep(1)

except KeyboardInterrupt:
    GPIO.cleanup
    osd_proc.terminate()
