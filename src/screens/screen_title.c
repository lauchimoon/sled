#include "sled_screens.h"
#include "sled_ui.h"
#include "sled_misc.h"
#include "sled_settings.h"
#include "sled_contextless.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define AMOUNT_BUTTONS      5
#define AMOUNT_TEXTBOXES    5
#define MAX_INPUT_SIZE      255
#define PATH_SIZE           512

static void textboxes_init(void);
static void draw_text_centered(SLED, const char *, int, int, Color);

static SledUIButton buttons[AMOUNT_BUTTONS] = { 0 };
static SledUITextBox textboxes[AMOUNT_TEXTBOXES] = { 0 };
static const char *texts[AMOUNT_BUTTONS] = {
    "New Map",
    "Load Map",
    "Exit",
    " X",
    "Ok"
};
static char *empty_texts[AMOUNT_TEXTBOXES] = {
    "Map Width",
    "Map Height",
    "Tile Width",
    "Tile Height",
    "Tileset path"
};
static bool show_new_dialog = false;
static bool show_no_file = false;
static int show_no_file_counter = 0;
static char input_map_width[MAX_INPUT_SIZE + 1] = "\0";
static char input_map_height[MAX_INPUT_SIZE + 1] = "\0";
static char input_tile_size_x[MAX_INPUT_SIZE + 1] = "\0";
static char input_tile_size_y[MAX_INPUT_SIZE + 1] = "\0";
static char input_tilesheet[MAX_INPUT_SIZE + 1] = "\0";
static char *inputs[AMOUNT_TEXTBOXES] = {
    input_map_width,
    input_map_height,
    input_tile_size_x,
    input_tile_size_y,
    input_tilesheet
};
static Rectangle frame = { 0 };

void sled_screen_title_init(SLED *sled)
{
    for (int i = 0; i < AMOUNT_BUTTONS; i++) {
        if (i < 3) { // Initialize buttons except the ones shown at the dialog box
            buttons[i].bounds = (Rectangle){ 15.0f, 70.0f + (i * 80), GetScreenWidth() - 32, 64.0f };
        }
        buttons[i].text = texts[i];
    }

    textboxes_init(); // Initialize textboxes
    sled->situation = SITUATION_NONE;
    show_new_dialog = false;
    show_no_file = false;
    show_no_file_counter = 0;
    frame = (Rectangle){ 20.0f, 20.0f, GetScreenWidth() - 40, GetScreenHeight() - 40 };
    buttons[3].bounds = (Rectangle){ 20.0f, 20.0f, 64.0f, 64.0f };
    buttons[4].bounds = (Rectangle){ 20.0f, 20.0f, frame.width - 20.0f, 64.0f };
}

void sled_screen_title_update(SLED *sled)
{
    frame = (Rectangle){ 20.0f, 20.0f, GetScreenWidth() - 40, GetScreenHeight() - 40 };
    for (int i = 0; i < AMOUNT_BUTTONS; i++) {
        if (i < 3) {
            buttons[i].bounds = (Rectangle){ 15.0f, 70.0f + (i * 80), GetScreenWidth() - 32, 64.0f }; // Update the values in case of resizing
        }
    }

    buttons[4].bounds = (Rectangle){ 30.0f, (frame.y + frame.height) - 76.0f, frame.width - 20.0f, 64.0f };

    if (!show_new_dialog) {
        if (sled_ui_button_pressed(buttons[0])) { // New Map
            show_new_dialog = true;
            sled->situation = SITUATION_NEW;
        }
        if (sled_ui_button_pressed(buttons[1])) { // Load Map
            sled->situation = SITUATION_LOAD;

            if (FileExists("map.sled")) {
                sled_set_current_screen(sled, SCREEN_EDIT);
            }
            show_no_file_counter = 0;
            show_no_file = true;
        }
        if (sled_ui_button_pressed(buttons[2])) { // Exit
            sled->exit = true;
        }
    } else {
        sled->tileset_path = inputs[4];
        if (sled_ui_button_pressed(buttons[3])) { // Close
            show_new_dialog = false;
        }
        if (sled_ui_button_pressed(buttons[4]) && FileExists(sled->tileset_path)) { // Ok
            int width = atoi(inputs[0]);
            int height = atoi(inputs[1]);
            int tx = atoi(inputs[2]); // Short for tile x
            int ty = atoi(inputs[3]); // Short for tile y
            char *tileset = inputs[4];
            char path[PATH_SIZE] = { 0 };
            bool valid = (width > 0 && height > 0 && tx > 0 && ty > 0);
            if (valid) {
                sled->map_width = width;
                sled->map_height = height;
                sled->tile_size_x = tx;
                sled->tile_size_y = ty;
                sled->tileset_path = tileset;
                FILE *f = fopen("sled_map_info.txt", "w");
                fprintf(f, "%d\n", sled->map_width);
                fprintf(f, "%d\n", sled->map_height);
                fprintf(f, "%d\n", sled->tile_size_x);
                fprintf(f, "%d\n", sled->tile_size_y);
                fprintf(f, "%s\n", pathof(sled->tileset_path, PATH_SIZE, path));
                fclose(f);

                sled_set_current_screen(sled, SCREEN_EDIT);
            }
        }
    }
}

void sled_screen_title_draw(SLED *sled)
{
    for (int i = 0; i < AMOUNT_BUTTONS; i++) {
        if (i < 3) {
            sled_ui_button_draw(*sled, buttons[i]);
        }
    }
    for (int i = 0; i < AMOUNT_TEXTBOXES; i++) {
        textboxes[i].bounds = (Rectangle){ 110.0f, 40.0f + (i * 60), GetScreenWidth() - 180, 48.0f };
    }
    draw_text(*sled, SLED_TITLE, 10, 10, 32, WHITE);
    draw_text(*sled, "Copyright (C) 2022 Laurentino Luna", 10, GetScreenHeight() - 32, 20, WHITE);
    draw_text(*sled, SLED_VERSION, 10, GetScreenHeight() - 58, 20, WHITE);
    if (show_no_file) {
        show_no_file_counter++;

        if (show_no_file_counter <= 90) {
            draw_text_centered(*sled, "No \"map.sled\" file found", GetScreenHeight() - 110, 20, WHITE);
        }
    }
    if (show_new_dialog) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));

        DrawRectangleRec(frame, LIGHTGRAY);
        DrawRectangleLinesEx(frame, 6, GRAY);
        sled_ui_button_draw(*sled, buttons[3]);
        sled_ui_button_draw(*sled, buttons[4]);

        for (int i = 0; i < AMOUNT_TEXTBOXES; i++) {
            sled_ui_textbox(*sled, &textboxes[i], inputs[i], MAX_INPUT_SIZE, (i != 4)); // All of the textboxes are numerical unless the index is 4
        }
    }
}

void sled_screen_title_deinit(SLED *sled)
{
}

static void textboxes_init(void)
{
    for (int i = 0; i < AMOUNT_TEXTBOXES; i++) {
        textboxes[i].bounds = (Rectangle){ 110.0f, 40.0f + (i * 60), GetScreenWidth() - frame.width, 48.0f };
        textboxes[i].over = false;
        textboxes[i].empty_text = empty_texts[i];
    }
#ifdef DEBUG
    inputs[0] = "32";
    inputs[1] = "21";
    inputs[2] = "32";
    inputs[3] = "32";
    inputs[4] = "../assets/sample_sheet.png";
#endif
}

static void draw_text_centered(SLED sled, const char *text, int y, int font_size, Color color)
{
    float spacing = sled.font.baseSize/font_size;
    int width = MeasureTextEx(sled.font, text, (float)font_size, spacing).x;
    draw_text(sled, text, GetScreenWidth()/2 - width/2, y, font_size, color);
}
