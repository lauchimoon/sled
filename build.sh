#!/bin/bash
# ______________________________________________________________________________
#
#  Compile raylib project
#
#  - Linux                   ./build.sh
#  - Windows (w64devkit)     ./build.sh
#  - Windows (cross compile) TARGET=Windows_NT ./build.sh
#
#  - Debug                   DEBUG=1 ./build.sh
#  - Build and run           ./build.sh -r
# ______________________________________________________________________________
#

# Default build options, override options from the command line

# Platform, one of Windows_NT, Linux, Web, Android. Defaults to your OS.
[[ "$TARGET" = "" ]] && TARGET=`uname`

# Executable name, extension is added depending on target platform.
[[ "$NAME" = "" ]] && NAME="sled"

# Compiler flags.
[[ "$FLAGS" = "" ]] && FLAGS=""

RELEASEFLAGS="-Os -flto -s"
DEBUGFLAGS="-O0 -g -Wall -Wextra -Wpedantic"

# ______________________________________________________________________________
#
#  Compile
# ______________________________________________________________________________
#
TYPEFLAGS=$RELEASEFLAGS
[[ "$DEBUG" != "" ]] && TYPEFLAGS=$DEBUGFLAGS

[[ -e lib/$TARGET ]] || ./setup.sh

case "$TARGET" in
	"Windows_NT")
		CC="x86_64-w64-mingw32-gcc"
		EXT=".exe"
		PLATFORM="PLATFORM_DESKTOP"
		TARGETFLAGS="-lopengl32 -lgdi32 -lwinmm -Wl,--subsystem,windows"
		;;

	"Linux")
		CC="gcc"
		PLATFORM="PLATFORM_DESKTOP"
		TARGETFLAGS="-lGL -lm -lpthread -ldl -lrt -lX11"
		;;

	*)
		echo "Unsupported OS $TARGET"
		exit 1
		;;
esac

$CC src/*.c src/screens/*.c -Iinclude -Llib/$TARGET -o $NAME$EXT \
	-lraylib -D$PLATFORM $FLAGS $TYPEFLAGS $TARGETFLAGS

# ______________________________________________________________________________
#
#  Run if -r was specified
# ______________________________________________________________________________
#
if [[ "$1" = "-r" ]]; then
	case "$TARGET" in
		"Windows_NT") ([[ `uname` = "Linux" ]] && wine $NAME$EXT) || $NAME$EXT;;
		"Linux") ./$NAME;;
	esac
fi