# Get pre-compiled kernel
https://github.com/kiteretro/rpi-precompiled-kernels

# Build
```
cd Circuit-Sword/build

chmod +x upgrade-kernel.sh build-image.sh

wget <base_image>.img.gz
gunzip <base_image>.img.gz

sudo ./upgrade-kernel.sh YES <base_image>.img pi_4.14.14-v7+.zip

sudo ./build-image.sh YES <base_image>_pi_4.14.14-v7+.img
```

Burn resulting .img to SD and boot!
