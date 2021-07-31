#!/bin/bash
set -e

# Install and Configure Bluetooth Audio Support for Pulse Audio
sudo apt-get install -y pulseaudio-module-bluetooth
sudo adduser pi bluetooth
if ! grep "load-module module-switch-on-connect" /etc/pulse/default.pa; then
  echo "load-module module-switch-on-connect" >> /etc/pulse/default.pa
fi

cat << EOF
BT Audio Support is ready to use.

Connect to the speaker through the GUI
Menu -> BLUETOOTH -> Register and Connect to Bluetooth Device
Select DisplayYesNo as agent

Have Pi automatically connect to the speaker
Menu -> BLUETOOTH -> Configure bluetooth connect mode
Set connect mode to "background"

Do a reboot
EOF 
