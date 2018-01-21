# Get pre-compiled kernel
https://github.com/geebles/rpi-precompiled-kernels

# Build
```
cd Circuit-Sword/build

chmod +x upgrade-kernel.sh build-image.sh

wget https://github.com/RetroPie/RetroPie-Setup/releases/download/4.3/retropie-4.3-rpi2_rpi3.img.gz
gunzip retropie-4.3-rpi2_rpi3.img.gz

sudo ./upgrade-kernel.sh YES retropie-4.3-rpi2_rpi3.img pi_4.14.14-v7+.zip

sudo ./build-image.sh YES retropie-4.3-rpi2_rpi3_pi_4.14.14-v7+.img
```

Burn resulting .img to SD and boot!
