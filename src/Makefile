CC = gcc
SRC = $(wildcard *.c screens/*.c)
RAYLIB_PATH = ../../raylib/
LDLIBS = -L$(RAYLIB_PATH)src/ -lraylib -lm -ldl -lpthread -lGL
INCLUDE = -I$(RAYLIB_PATH)src/ -Iinclude/
CFLAGS = -Wall -g
DEBUG = -DNO
OUT = sled
INVOKE = $(CC) $(SRC) $(LDLIBS) $(INCLUDE) $(CFLAGS) $(DEBUG) -o $(OUT)

default:
	$(INVOKE)

run:
	$(INVOKE)
	./$(OUT)
	rm $(OUT)

clean:
	rm $(OUT)
