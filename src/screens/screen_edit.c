#include "sled_screens.h"
#include "sled_misc.h"
#include "sled_ui.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define NEZ_TILESET_IMPLEMENTATION
#include "tileset.h"

#define NEZ_TILEMAP_IMPLEMENTATION
#include "tilemap.h"

#define MOUSE_SENSITIVITY   0.8f
#define SCROLL_SPEED        8
#define CMD_SIZE            255

static void input(SLED);
static void export_binary_map(SLED, const char *);
static void export_header_map(SLED, const char *);

static Camera2D camera = { 0 };
static Vector2 last_mouse_pos = { 0 };
static Vector2 current_tile = { 0 };
static Texture tilesheet = { 0 };
static TileMap *tilemap = { 0 };
static TileSet *tileset = { 0 };
static SledUITextBox cmd_textbox = { 0 };
static Rectangle cmd_textbox_bounds = { 0 };
static char textbox_content[CMD_SIZE + 1] = "\0";

static int tileset_display = 0;
static int tileset_index = 0;
static unsigned char *map;
static unsigned char *save_data;
static unsigned char *loaded_data;
static bool flag_grid = true;
static bool flag_pan = false;
static int first_index = 0;
static int last_index = 0;
static int frames_counter = 0;
static char *map_info = "";
static const char **map_info_split;
static int map_width = 0;
static int map_height = 0;
static int map_size = 0;
static int map_tile_size_x = 0;
static int map_tile_size_y = 0;
static const char *tileset_path = "";
static int cmd_text_index = 0;
static const char *cmd_texts[3] = {
    "",
    "Exported map to binary!",
    "Exported map to header file!"
};
static bool show_cmd_text = false;
static int show_cmd_counter = 0;

void sled_screen_edit_init(SLED *sled)
{
    current_tile = (Vector2){ 0.0f, 0.0f };
    camera.offset = (Vector2){ 32*2, 32 };
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.5f;

    last_mouse_pos = (Vector2){ 0.0f, 0.0f };

    tileset_display = 0;
    tileset_index = 0;
    flag_grid = true;
    flag_pan = false;
    frames_counter = 0;
    map_info = LoadFileText("sled_map_info.txt");
    int split_count = 0;
    map_info_split = TextSplit(map_info, '\n', &split_count);

    map_width = atoi(map_info_split[0]);
    map_height = atoi(map_info_split[1]);
    map_size = map_width*map_height;
    map_tile_size_x = atoi(map_info_split[2]);
    map_tile_size_y = atoi(map_info_split[3]);
    tileset_path = map_info_split[4];
    assert(FileExists(tileset_path));

    tilesheet = LoadTexture(tileset_path);
    first_index = 0;
    last_index = tilesheet.width/map_tile_size_x - 1; // Last tileset index so we can wrap the index around
    size_t size = (map_size)*sizeof(unsigned char);

    if (!sled->situation) { // SITUATION_NEW
        map = calloc(map_size, size);
    }

    // Tilemap & tilesheet stuff
    tileset = TileSetNewInitFromMemory(tilesheet, map_tile_size_x, map_tile_size_y, NULL, 0);
    tilemap = TileMapNew();
    tilemap->tileSet = tileset;
    tilemap->x = 0;
    tilemap->y = 0;
    TileMapInitSize(tilemap, map_width, map_height);
    
    if (sled->situation) { // SITUATION_LOAD
        unsigned int loaded_bytes = 0;
        loaded_data = LoadFileData("map.sled", &loaded_bytes);
        map = calloc(map_size, size);

        for (int i = 0; i < map_size; i++) {
            map[i] = loaded_data[i];
        }
    }

    // Allocate the save_data map anyways
    // The reason why we do this is to avoid writing non-allocated memory
    // even in a SITUATION_LOAD situation.
    save_data = calloc(map_size, size);
    for (int i = 0; i < map_size; i++) {
        save_data[i] = map[i];
    }

    // Set the map's loaded data (or new) to the renderable map
    for (int i = 0; i < map_size; i++) {
        tilemap->grid[i] = (int)map[i];
    }

    // Command box stuff
    cmd_textbox_bounds = (Rectangle){ 10, 40, 220, 48 };
    cmd_textbox.bounds = cmd_textbox_bounds;
    cmd_textbox.over = false;
    cmd_textbox.letter_count = 0;
    cmd_textbox.empty_text = "Type a command";

    cmd_text_index = 0;
    show_cmd_text = false;
    show_cmd_counter = 0;
}

void sled_screen_edit_update(SLED *sled)
{
    input(*sled);
    cmd_textbox.empty_text = "Type a command";
}

void sled_screen_edit_draw(SLED *sled)
{
    BeginMode2D(camera);
        Rectangle current_tile_rec = { current_tile.x*map_tile_size_x, current_tile.y*map_tile_size_y, map_tile_size_x, map_tile_size_y }; // Cursor
        TileMapDraw(tilemap);
        TileMapDrawGrid(tilemap, flag_grid? WHITE : BLANK);
        DrawRectangleLinesEx(current_tile_rec, 2, GREEN);
    EndMode2D();

    Vector2 tile_pos = { (10 + map_tile_size_x) + 30, GetScreenHeight() - (map_tile_size_y + map_tile_size_y + 1) }; // Where the tile preview will be rendered
    Rectangle tile_subtexture = { tileset_display, 0, map_tile_size_x, map_tile_size_y };
    Rectangle tile_frame = { tile_pos.x, tile_pos.y, map_tile_size_x, map_tile_size_y };

    draw_text(*sled, "Tile:", 10, GetScreenHeight() - 60, 20, WHITE);
    DrawTextureRec(tilesheet, tile_subtexture, tile_pos, WHITE);
    DrawRectangleLinesEx(tile_frame, 1, BLACK);

    draw_text(*sled, TextFormat("Cursor: %d,%d", (int)current_tile.x, (int)current_tile.y), 10, GetScreenHeight() - 30, 20, WHITE);
    draw_text(*sled, TextFormat("Zoom: %.0f%%", camera.zoom*100), 10, 10, 20, WHITE);
    sled_ui_textbox(*sled, &cmd_textbox, textbox_content, CMD_SIZE, false);
    draw_text(*sled, cmd_texts[cmd_text_index], (cmd_textbox.bounds.x + cmd_textbox.bounds.width) + 20, 50, 20, (show_cmd_text)? WHITE : BLANK);
}

void sled_screen_edit_deinit(SLED *sled)
{
    TileMapDestroy(tilemap);
    TileSetDestroy(tileset);
    UnloadTexture(tilesheet);
    free(map);
    free(save_data);
    UnloadFileData(loaded_data);
    free(map_info);
}

// -----
static void input(SLED sled)
{
    // Cursor move
    current_tile.x = (int)GetScreenToWorld2D(GetMousePosition(), camera).x/map_tile_size_x;
    current_tile.y = (int)GetScreenToWorld2D(GetMousePosition(), camera).y/map_tile_size_y;

    // Tileset index increase or decrease
    tileset_display += map_tile_size_x*IsKeyPressed(KEY_V);
    tileset_display -= map_tile_size_x*IsKeyPressed(KEY_C);

    if (IsKeyPressed(KEY_V)) {
        tileset_index += 1;
        if (tileset_index > last_index) {
            tileset_index = first_index;
        }
    }
    if (IsKeyPressed(KEY_C)) {
        tileset_index -= 1;
        if (tileset_index < 0) {
            tileset_index = last_index;
        }
    }

    // Zoom in or out
    camera.zoom += (GetMouseWheelMove()*SCROLL_SPEED)/100;
    if (camera.zoom <= 0.7f) {
        camera.zoom = 0.7f;
    }
    if (camera.zoom >= 2.9f) {
        camera.zoom = 2.9f;
    }

    // Set the grid
    if (IsKeyPressed(KEY_G)) {
        flag_grid = !flag_grid;
    }

    // Panning
    flag_pan = IsKeyDown(KEY_LEFT_SHIFT);

    if (flag_pan) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            last_mouse_pos = GetMousePosition();
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse_delta = GetMouseDelta();
            camera.target.x -= mouse_delta.x*MOUSE_SENSITIVITY;
            camera.target.y -= mouse_delta.y*MOUSE_SENSITIVITY;
        }
    }

    // Change tile
    if (IsKeyDown(KEY_SPACE) || (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !flag_pan)) {
        TileMapSetTile(tilemap, (int)current_tile.x, (int)current_tile.y, tileset_index);
    }

    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && !flag_pan) {
        TileMapSetTile(tilemap, (int)current_tile.x, (int)current_tile.y, -1);
    }

    // Command box
    if (IsKeyPressed(KEY_ENTER)) {
        show_cmd_text = true;
        if (TextIsEqual(textbox_content, "tobin")) {
            export_binary_map(sled, "map.sled");
            cmd_text_index = 1;
        } else if (TextIsEqual(textbox_content, "toheader")) {
            export_header_map(sled, "sled_map.h");
            cmd_text_index = 2;
        } else {
            cmd_text_index = 0;
        }
    }

    if (show_cmd_text) {
        show_cmd_counter++;

        if (show_cmd_counter >= 90) {
            show_cmd_counter = 0;
            show_cmd_text = false;
        }
    }
}

static void export_binary_map(SLED sled, const char *output_file)
{
    for (int i = 0; i < map_size; i++) {
        save_data[i] = tilemap->grid[i];
    }
    SaveFileData(output_file, save_data, sizeof(unsigned char)*map_width*map_height);
}

static void export_header_map(SLED sled, const char *output_file)
{
    for (int i = 0; i < map_size; i++) {
        save_data[i] = tilemap->grid[i];
    }
    FILE *f = fopen(output_file, "w");
    fprintf(f, "int sled_map[%d] = {\n", map_size);
    fprintf(f, "    ");

    for (int i = 0; i < map_size; i++) {
        fprintf(f, "%d, ", (int)save_data[i]);
        if ((i + 1) % map_width == 0) {
            fprintf(f, "\n    ");
        }
    }
    fprintf(f, "\n};\n");

    fclose(f);
}

