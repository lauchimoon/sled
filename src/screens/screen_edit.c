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
#define AMOUNT_BUTTONS      2

static void input(SLED);
static void export_binary_map(SLED, const char *);
static void export_header_map(SLED, const char *);
static void draw_render_texture(RenderTexture2D);

static Camera2D camera = { 0 };
static Vector2 last_mouse_pos = { 0 };
static Vector2 current_tile = { 0 };
static Texture tilesheet = { 0 };
static TileMap *tilemap = { 0 };
static TileSet *tileset = { 0 };
static Rectangle frame_rec = { 0 };
static SledUIButton buttons[AMOUNT_BUTTONS] = { 0 };
static char *button_texts[AMOUNT_BUTTONS] = {
    "Export to sled map",
    "Export to header file"
};

static RenderTexture2D target = { 0 };
static int sq_width = 0;
static int sq_height = 0;

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

void sled_screen_edit_init(SLED *sled)
{
    current_tile = (Vector2){ 0.0f, 0.0f };
    camera.offset = (Vector2){ 10.0f, 10.0f };
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.5f;

    last_mouse_pos = (Vector2){ 0.0f, 0.0f };

    tileset_display = 0;
    tileset_index = 0;
    flag_grid = true;
    flag_pan = false;
    frames_counter = 0;
    map_info = LoadFileText(sled->map_info_file);
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
        loaded_data = LoadFileData(sled->map_file, &loaded_bytes);
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

    // Rendering square
    sq_width = (GetScreenWidth()/2) + 180;
    sq_height = GetScreenHeight() - 60;
    target = LoadRenderTexture(sq_width, sq_height);
    frame_rec = (Rectangle){ 10.0f, 10.0f, sq_width - map_tile_size_x/3, sq_height };

    // Buttons
    for (int i = 0; i < AMOUNT_BUTTONS; i++) {
        buttons[i] = (SledUIButton){
            .bounds = (Rectangle){ (frame_rec.x + frame_rec.width) + 20, 40.0f, 180, 48 },
        };
    }
}

void sled_screen_edit_update(SLED *sled)
{
    input(*sled);

    // Rendering square
    sq_width = (GetScreenWidth()/2) + 180;
    sq_height = GetScreenHeight() - 60;

    if (IsWindowResized()) {
        UnloadRenderTexture(target);
        target = LoadRenderTexture(sq_width, sq_height);
    }
    frame_rec = (Rectangle){ 10.0f, 10.0f, sq_width - map_tile_size_x/3, sq_height };

    // Buttons
    buttons[0].bounds = (Rectangle){ (frame_rec.x + frame_rec.width) + 20, 40.0f, GetScreenWidth() - 100, 64.0f };
    buttons[1].bounds = (Rectangle){ (frame_rec.x + frame_rec.width) + 20, 145.0f, GetScreenWidth() - 100, 64.0f };
}

void sled_screen_edit_draw(SLED *sled)
{
    BeginTextureMode(target);
        ClearBackground(BLANK);
        BeginMode2D(camera);
            Rectangle current_tile_rec = { current_tile.x*map_tile_size_x, current_tile.y*map_tile_size_y, map_tile_size_x, map_tile_size_y }; // Cursor
            TileMapDraw(tilemap);
            TileMapDrawGrid(tilemap, flag_grid? WHITE : BLANK);
            DrawTextureRec(tilesheet, (Rectangle){ tileset_display, 0.0f, map_tile_size_x, map_tile_size_y }, (Vector2){ current_tile_rec.x, current_tile_rec.y }, Fade(WHITE, 0.9f));
            DrawRectangleLinesEx(current_tile_rec, 2, GREEN);
        EndMode2D();
    EndTextureMode();

    draw_render_texture(target); // NOTE: this function is not portable. Implementation depends on what the programmer wants to do.
    DrawRectangleLinesEx(frame_rec, 3, LIME); // Draw a frame around the grid

    draw_text(*sled, TextFormat("Cursor: %d,%d", (int)current_tile.x, (int)current_tile.y), 10, GetScreenHeight() - 30, 20, WHITE);
    draw_text(*sled, TextFormat("Zoom: %.0f%%", camera.zoom*100), GetScreenWidth() - 150, 10, 20, WHITE);
    sled_ui_button_draw(*sled, buttons[0]);
    draw_text(*sled, "Export to sled map", buttons[0].bounds.x + 20, buttons[0].bounds.y + 18, 20, WHITE);
    sled_ui_button_draw(*sled, buttons[1]);
    draw_text(*sled, "Export to header", buttons[1].bounds.x + 20, buttons[1].bounds.y + 18, 20, WHITE);
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
    UnloadRenderTexture(target);
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
    if (CheckCollisionPointRec(GetMousePosition(), frame_rec)) {
        camera.zoom += (GetMouseWheelMove()*SCROLL_SPEED)/100;
    }
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
    if (CheckCollisionPointRec(GetMousePosition(), frame_rec)) {
        flag_pan = IsKeyDown(KEY_LEFT_SHIFT);
    }

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
    if (CheckCollisionPointRec(GetMousePosition(), frame_rec)) {
        if (IsKeyDown(KEY_SPACE) || (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !flag_pan)) {
            TileMapSetTile(tilemap, (int)current_tile.x, (int)current_tile.y, tileset_index);
        }

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && !flag_pan) {
            TileMapSetTile(tilemap, (int)current_tile.x, (int)current_tile.y, -1);
        }
    }

    // Exporting
    if (sled_ui_button_pressed(buttons[0])) {
        export_binary_map(sled, "map.sled");
    }
    if (sled_ui_button_pressed(buttons[1])) {
        export_header_map(sled, "sled_map.h");
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

static void draw_render_texture(RenderTexture2D rt)
{
    DrawTextureRec(rt.texture, (Rectangle){ 0.0f, 0.0f, (float)rt.texture.width, (float)-rt.texture.height }, (Vector2){ 0.0f, 0.0f }, WHITE);
}

