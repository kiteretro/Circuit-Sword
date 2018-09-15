#!/usr/bin/env python

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

import os,sys
import re
import argparse

# Config variables
config_path = "/boot/config.txt"

class HDMI_Switcher:
  
  settings = {
    'VERSION': "# CS CONFIG VERSION: ",
    'STATE': "# CS CONFIG STATE: ",
    'HDMI_START': "# CS START HDMI SETTINGS",
    'HDMI_END': "# CS END HDMI SETTINGS",
    'DPI_START': "# CS START DPI SETTINGS",
    'DPI_END': "# CS END DPI SETTINGS"
  }
  
  states = {
    'IDLE': "IDLE",
    'IN_PROGRESS': "REBOOTING_TO_HDMI"
  }
  
  def __init__(self, config_path):
    self.config_path = config_path
    self.config = None
    self.version = None
    self.state = None
    self.load_config_file()
    self.changed = False
    print("[i] Completed load (version: %s, state: %s)" % (self.version, self.state))

  # Read config file
  def load_config_file(self):
    # Load the file
    print("[*] Reading config file '%s'" % self.config_path)
    self.config = []
    self.version = None
    self.state = None
    self.changed = False
    with open(self.config_path, 'r') as f:
      self.config = f.read().splitlines()
    
    # Validate content
    for k,v in self.settings.iteritems():
      found_count = 0
      
      for line in self.config:
        if v in line:
          found_count += 1
      
      if found_count != 1:
        assert found_count == 1, "ERROR: Couldn't find '%s' in '%s'" % (v, self.config_path)
    print("[i] Validation succeeded")
    
    # Load properties
    self.version = self.read_property(self.settings['VERSION'])
    assert self.version is not None, "ERROR: Couldn't determine VERSION"
    
    self.state = self.read_property(self.settings['STATE'])
    assert self.state is not None, "ERROR: Couldn't determine STATE"
    
    return True
  
  # Save config file
  def save_config_file(self):
    if self.changed:
      print("[*] Writing config file '%s'" % self.config_path)
      with open(self.config_path, 'w') as f:
        for line in self.config:
          f.write("%s\n" % line)
      self.changed = False
    else:
      print("[i] No changes made, not writing config file")
    return True

  # Read property
  def read_property(self, prop):
    for line in self.config:
      match = re.match("^%s(.*)$" % prop, line)
      if match:
        print("[i] Found property '%s' as '%s'" % (prop, match.group(1)))
        return match.group(1)
    return None
  
  # Write property
  def write_property(self, prop, value):
    print("[*] Writing property '%s%s'" % (prop, value))
    line_index = 0
    for line in self.config:
      match = re.match("^%s(.*)$" % prop, line)
      if match:
        self.config[line_index] = "%s%s" % (prop, value)
        self.changed = True
        return True
      line_index += 1
    assert True, "ERROR: Could not find and write property '%s' with value '%s'" % (prop, value)
    return True
    
  # Comment/uncomment a block of code
  def process_section(self, section, comment=True):
    print("[*] Processing section '%s' (comment=%s)" % (section, comment))
    
    if section == 'HDMI':
      start = self.settings['HDMI_START']
      end = self.settings['HDMI_END']
    elif section == 'DPI':
      start = self.settings['DPI_START']
      end = self.settings['DPI_END']
    else:
      raise Exception("Invalid section '%s'" % section)
    
    line_index = 0
    found_start = False
    for line in self.config:
      # Search for start
      if start in line:
        found_start = True
        
      elif found_start:
        # Check for end
        if end in line:
          break
        
        # Only populated lines
        if line:        
        
          # Process the action!
          if comment:
            self.config[line_index] = "#%s" % line
            self.changed = True
          else:
            match = re.match("^#(.*)", line)
            if match:
              self.config[line_index] = match.group(1)
              self.changed = True
            else:
              raise Exception("Invalid format: '#' was not at start of line %s: '%s'" % (line_index, line))
      
      line_index += 1
    
    assert found_start, "ERROR: Could not process section '%s'" % section
    return True
  
  # Enable HDMI
  def enable_hdmi(self):
    self.process_section('HDMI', comment=False)
    self.process_section('DPI', comment=True)
    self.write_property(self.settings['STATE'], self.states['IN_PROGRESS'])
    return True
  
  # Enable DPI
  def enable_dpi(self):
    self.process_section('HDMI', comment=True)
    self.process_section('DPI', comment=False)
    self.write_property(self.settings['STATE'], self.states['IDLE'])
    return True
  
  # Check and repair
  def enable_check(self):
    if self.state == self.states['IN_PROGRESS']:
      print("[*] Restoring initial state (DPI)")
      self.enable_dpi()
    elif self.state == self.states['IDLE']:
      print("[i] State already idle, no changes needed")
    else:
      raise Exception("Unknown state '%s' so cannot action" % self.state)
    return True

if __name__ == '__main__':
  
  # Read arguments
  parser = argparse.ArgumentParser(description='HDMI SWITCHER')
  parser.add_argument('--hdmi', default=False, action='store_true', help='Enable HDMI')
  parser.add_argument('--dpi', default=False, action='store_true', help='Enable DPI')
  parser.add_argument('--check', default=False, action='store_true', help='Check the correct settings and repair them')
  parser.add_argument('--dryrun', default=False, action='store_true', help='Dont save, just print to screen')
  parser.add_argument('--reboot', default=False, action='store_true', help='Do the reboot on success')
  parser.add_argument('--config', default=config_path, help="Set the config path (default: %s)" % config_path)
  args = parser.parse_args()  
  
  # Create and init the switching object
  Switcher = HDMI_Switcher(args.config)
  
  # Action the command
  if args.hdmi:
    Switcher.enable_hdmi()
  elif args.dpi:
    Switcher.enable_dpi()
  elif args.check:
    Switcher.enable_check()
  else:
    print("[!] ERROR: No command was specified. Run with --help to see options")
    sys.exit(1)
  
  # Apply actions (or not)
  if args.dryrun:
    # Print to screen
    if Switcher.changed:
      for line in Switcher.config:
        print(line)
    else:
      print("[d] No changes were made to config so nothing to show")
  else:
    # Write file
    Switcher.save_config_file()
    # Reboot
    if args.reboot:
      os.system("sudo reboot")
