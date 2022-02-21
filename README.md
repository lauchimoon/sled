# SLED
Stupid Level EDitor

**The program is at a very WIP stage.**

## Features
- Small: a relatively flexible editor to make maps
- Export functionality: export to a `.sled` file or a header file
- Easy to use

## Usage

### Running the program
SLED can be called directly with no arguments and it will load up the "UI mode", which will ask either for a new map or ask you to load a map

However, SLED can also be used from the command line.

Calling
```
sled new
```
Will open up a small interface which will ask for input, then after all fields are completed, the program will run.

Calling
```
sled load
```
Will look for a `map.sled` file and a `sled_map_info.txt` file on the current working directory.

### Controls
- Use your left mouse button to set tiles
- Use your right mouse button to clear tiles
- Use the mouse wheel to zoom in or out
- Hold shift and use the left mouse button to pan over the map
- Use C and V to change the tileset index (this will be changed)

### Exporting
SLED has a vim-like way to export your maps. Inside the editor, there is a small textbox which will ask for commands. The commands are
```
tobin
toheader
```

## Building
You will need raylib and GNU make.
```
$ git clone https://github.com/catmanl/sled.git
$ cd sled/
$ cd src/
$ make
```

## Coming soon
SLED on Windows.

SLED will have an api to load maps into your games. Although you can already use header files, it would be expensive in case it's too big of a map.

## Credits
Font - Ubuntu Mono R

## License
sled is licensed under the GPL-2.0 license.

