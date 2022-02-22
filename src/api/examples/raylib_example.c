#include "raylib.h"
#include "sled_api.h"

// Helper functions to draw tilemap
#define NEZ_TILEMAP_IMPLEMENTATION
#include "tilemap.h"
#define NEZ_TILESET_IMPLEMENTATION
#include "tileset.h"

int main()
{
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 450, "SLED API - raylib");
    sled_map map = sled_load_map("map.sled", "sled_map_info.txt");
    Texture2D tilesheet = LoadTexture(map.tilesheet);

    TileSet *tileset = TileSetNewInitFromMemory(tilesheet, map.tile_width, map.tile_height, NULL, 0);
    TileMap *tilemap = TileMapNew();
    tilemap->tileSet = tileset;
    tilemap->x = 0;
    tilemap->y = 0;
    TileMapInitSize(tilemap, map.width, map.height);

    for (int i = 0; i < map.width*map.height; i++) {
        tilemap->grid[i] = (int)map.grid[i];
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        TileMapDraw(tilemap);
        EndDrawing();
    }

    TileMapDestroy(tilemap);
    TileSetDestroy(tileset);
    UnloadTexture(tilesheet);
    sled_free_map(map);
    CloseWindow();
    return 0;
}
