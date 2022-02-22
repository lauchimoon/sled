#include "sled_api.h"
#include <stdio.h>

int main()
{
    sled_map map = sled_load_map("map.sled", "sled_map_info.txt");

    printf("Map width: %d, Map height: %d, Tile width: %d, Tile height: %d, Sheet path: %s\n", map.width, map.height, map.tile_width, map.tile_height, map.tilesheet);

    sled_free_map(map);
    return 0;
}
