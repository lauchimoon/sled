#include "sled_context.h"
#include "sled_misc.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(int argc, char **argv)
{
    SLED sled;

    if (argc < 2 || argc > 2) {
        sled.ui = true;
    } else {
        if (strcmp(argv[1], "new") == 0) {
#ifndef DEBUG
            int map_width;
            int map_height;
            int tile_size_x;
            int tile_size_y;
            char tileset_path[] = { 0 };

            scan_int("[SLED]: Map width: ", &map_width);
            assert(map_width > 0);
            scan_int("[SLED]: Map height: ", &map_height);
            assert(map_height > 0);
            scan_int("[SLED]: Tile size x: ", &tile_size_x);
            assert(tile_size_x > 0);
            scan_int("[SLED]: Tile size y: ", &tile_size_y);
            assert(tile_size_y > 0);
            scan_str("[SLED]: Tileset path: ", tileset_path);
            assert(tileset_path != NULL);

            // Apply the values inputted to the configuration
            sled.map_width = map_width;
            sled.map_height = map_height;
            sled.tile_size_x = tile_size_x;
            sled.tile_size_y = tile_size_y;
            sled.tileset_path = tileset_path;
#else
            sled.map_width = 1;
            sled.map_height = 1;
            sled.tile_size_x = 1;
            sled.tile_size_y = 1;
            sled.tileset_path = "../assets/sample_sheet.png";
#endif
            FILE *f = fopen("sled_map_info.txt", "w");
            fprintf(f, "%d\n", sled.map_width);
            fprintf(f, "%d\n", sled.map_height);
            fprintf(f, "%d\n", sled.tile_size_x);
            fprintf(f, "%d\n", sled.tile_size_y);
            fprintf(f, "%s\n", sled.tileset_path);
            fclose(f);
            sled.situation = SITUATION_NEW;
        } else if (strcmp(argv[1], "load") == 0) {
            assert(FileExists("map.sled")); // TODO: ask for the map.sled location
            sled.situation = SITUATION_LOAD;
        } else {
            sled.ui = true;
        }
    }

    sled_init(&sled);

    while (sled_run(sled)) {
        sled_update(&sled);
    }

    sled_close(&sled);
    return 0;
}

