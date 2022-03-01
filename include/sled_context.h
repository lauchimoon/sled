#ifndef sled_context_h
#define sled_context_h

#include <stdbool.h>
#include "raylib.h"

typedef enum {
    SITUATION_NEW = 0,
    SITUATION_LOAD,
    SITUATION_NONE
} Situations;

typedef struct SLED {
    int screen;
    Font font;
    bool exit;
    bool ui;

    int map_width;
    int map_height;
    int tile_size_x;
    int tile_size_y;
    char *tileset_path;
    int situation;
    char *map_file;
    char *map_info_file;
} SLED;

void sled_init(SLED *);
bool sled_run(SLED);
void sled_update(SLED *);
void sled_close(SLED *);

#endif // sled_h
