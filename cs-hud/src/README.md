# Circuit-Sword-HUD
Project to hold source files for HUD

## Implementation Notes
* Basic USE defines
* Generally geared for CSO
* Added serialport module - works
* Added more state info and debug output of sorts

## Git notes
* git push -u origin master

## Building
This is a simple project, and the build target is configured in config.h with the defined lines:

```
#define MODEL MODEL_CIRCUIT_SWORD
// #define MODEL MODEL_CIRCUIT_SWORD_LITE
// #define MODEL MODEL_CIRCUIT_GEM
```

Once set, run `./rebuild.sh`. This was designed to be run ON a raspberry Pi, and ideally the build target (e.g. CM3)
 
### Rebuilding icons
The icons are PNGs that are then converted into C includes so that file loading is not needed.  There is a converter in `icon_src` that you can run with `python convert_png_to_ch.py`. You will first need to set up the dirs in that python file, e.g. `root_dir = "/home/pi/Circuit-Sword-HUD/display"` is the default.
