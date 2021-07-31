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

if [ $# != 2 ] && [ $# != 3 ] ; then
  echo "Usage: ./<cmd> YES <image.img> [branch]"
  exit 1
fi

#####################################################################
# Vars

if [[ $2 != "" ]] ; then
  IMG=$2
fi

if [[ $3 != "" ]] ; then
  BRANCH=$3
else
  BRANCH="master"
fi

BUILD="CSO_CM3_"$(date +"%Y%m%d-%H%M%S")
GITHUBPROJECT="Circuit-Sword"
GITHUBURL="https://github.com/Antho91/$GITHUBPROJECT"
PIHOMEDIR="/home/pi"
BINDIR="$PIHOMEDIR/$GITHUBPROJECT"
USER="pi"

MOUNTFAT32="/mnt/fat32"
MOUNTEXT4="/mnt/ext4"

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

#####################################################################
# LOGIC!
echo "BUILDING.."

# Sanity check IMG
if ! exists $IMG ; then
  echo "ERROR: IMG [$IMG] doesn't exist"
  exit 1
fi

OUTFILE=$(basename $IMG .img)"_$BUILD.img"
ZIPFILE=$(basename $IMG .img)"_$BUILD.zip"

# Sanity check OUTFILE
if exists $OUTFILE ; then
  echo "ERROR: OUTFILE [$OUTFILE] exists! Can't create new image"
  exit 1
fi

# Check the mounted dir is clean
MNTDIRCLEANCOUNT=$(ls $MOUNTFAT32 | wc -l)
if [ $MNTDIRCLEANCOUNT != 0 ] ; then
  echo "ERROR: Mount dir [$MOUNTFAT32] is not empty [$MNTDIRCLEANCOUNT], perhaps something is mounted on it?"
  exit 1
fi

# Check the mounted dir is clean
MNTDIRCLEANCOUNT=$(ls $MOUNTEXT4 | wc -l)
if [ $MNTDIRCLEANCOUNT != 0 ] ; then
  echo "ERROR: Mount dir [$MOUNTEXT4] is not empty [$MNTDIRCLEANCOUNT], perhaps something is mounted on it?"
  exit 1
fi

# Copy img to new + name
execute "cp $IMG $OUTFILE"

# Find partions using kpartx
execute "kpartx -a -v -s $OUTFILE" 

# Mount partitions
execute "sudo mount /dev/mapper/loop0p1 $MOUNTFAT32"
execute "sudo mount /dev/mapper/loop0p2 $MOUNTEXT4"

# Install
execute "../install.sh YES $BRANCH $MOUNTFAT32 $MOUNTEXT4"

# Patch for PLUS 
# Not needed for Debian buster
# execute "unzip -o -d $MOUNTFAT32 $MOUNTEXT4$BINDIR/settings/pi_plus_20190130.zip"

# Unmount partitions
execute "umount $MOUNTFAT32"
execute "umount $MOUNTEXT4"

# Remove mapped partitions
execute "kpartx -d -v $OUTFILE"

# DONE
echo "SUCCESS: Image [$OUTFILE] has been built!"
