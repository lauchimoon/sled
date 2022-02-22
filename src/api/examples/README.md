# How to compile these examples
`cli_example.c`:
```
$ gcc cli_example.c ../sled_api.c -I../ -o cli_example
```

`raylib_example.c`:

**Assumes you have previously set up raylib to build sled**

Linux:
```
$ gcc raylib_example.c ../sled_api.c -I../ -I../../../include/ -L../../../lib/Linux/ -lraylib -lGL -lm -ldl -lpthread -o raylib_example
```

Windows (using w64devkit):
```
$ gcc raylib_example.c ../sled_api.c -I../ -I../../../include/ -L../../../lib/Windows_NT/ -lraylib -lopengl32 -lgdi32 -lwinmm -Wl,--subsystem,windows -o raylib_example.exe
```

There are some sample files so it should render the following:
![]("https://github.com/catmanl/sled/blob/main/src/api/examples/rl_screenshot.png")
