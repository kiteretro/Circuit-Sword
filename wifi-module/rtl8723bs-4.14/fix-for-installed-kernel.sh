#!/bin/bash
sed -e

#This script tries to fix the wifi module

# extract kernel version from zimage file - currently installed kernel

# kernel name
if [ "$(uname -m)" == "armv6l" ]; then file=/boot/kernel.img; else file=/boot/kernel7.img; fi
echo "kernel = $file"

# get offset of start of gzip file, marked by 0x1f 0x8b
skip=$(hexdump -ve '"%_ad " 1/1 "%02x\n"' $file |grep -B1 '8b' |grep -m1 '1f' |awk '{print $1;}')
echo "offset = $skip bytes"

# unzip and scan for strings
line=$(dd if=$file bs=1 skip=$skip |gunzip |strings |grep -iPm1 'Linux version \d\.\d+\.\d+')
version=$(perl -ne 'print "$1\n" if /\s(\d+\.\d+\.\d+\S+)\s/' <<<$line)
echo "headers = $version"

if [ ! -d "/lib/modules/$version/build" ]; then
	echo "Headers not found!"
	echo "Please install kernel headers $version"
	exit 1
fi

KERNELRELEASE=$version make
KERNELRELEASE=$version make modules_install
depmod -a
