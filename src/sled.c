#include "sled_settings.h"
#include "sled_context.h"
#include "sled_screens.h"

#include "../assets/font.h" // embedded font
#include "../assets/logo.h"
#include <stddef.h>
#include <stdio.h>

void sled_init(SLED *sled)
{
    // raylib setup
    SetTraceLogLevel(LOG_NONE);
    InitWindow(SLED_WINDOW_WIDTH, SLED_WINDOW_HEIGHT, SLED_TITLE);
    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);

    Image image = LoadImageFromMemory(".png", logo_png, logo_png_len);
    SetExitKey(KEY_NULL);
    SetWindowMinSize(SLED_WINDOW_WIDTH, SLED_WINDOW_HEIGHT);
    sled->screen = SCREEN_TITLE;
    SetWindowIcon(image);
    UnloadImage(image);

    if (!sled->ui) {
        sled->screen = SCREEN_EDIT;
    }
    sled->font = LoadFontFromMemory(".ttf", font_ttf, font_ttf_len, 64, NULL, 95);
    sled->exit = false;

    sled_screen_init(sled, sled->screen);

    SetTargetFPS(60);
}

bool sled_run(SLED sled)
{
    return !sled.exit;
}

void sled_update(SLED *sled)
{
    sled->exit = WindowShouldClose(); // so that we can end the loop on pressing an exit button
    sled_screen_update(sled, sled->screen);

    BeginDrawing();
        ClearBackground(DARKGRAY);
        sled_screen_draw(sled, sled->screen);
    EndDrawing();
}

void sled_close(SLED *sled)
{
    sled_screen_deinit(sled, sled->screen);
    UnloadFont(sled->font);
    CloseWindow();
}

