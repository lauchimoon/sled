CC = gcc
SRC ?= raylib_example.c
API_PATH = ../sled_api.c
INCLUDE_PATH = -I../
OUT = output

ifeq ($(SRC),raylib_example.c)
	INCLUDE_PATH += -I../../../include
	ifeq ($(OS),Windows_NT)
		LDLIBS = -L../../../lib/Windows_NT/ -lraylib -lopengl32 -lgdi32 -lwinmm -Wl,--subsystem,windows
	else
		LDLIBS = -L../../../lib/Linux/ -lraylib -lGL -lm -ldl -lpthread
	endif
endif


default:
	$(CC) $(SRC) $(API_PATH) $(INCLUDE_PATH) $(LDLIBS) -o $(OUT)
