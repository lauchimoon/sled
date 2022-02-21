#include "sled_context.h"
#include "sled_screens.h"

void sled_screen_init(SLED *sled, int screen)
{
    switch (screen) {
        case SCREEN_TITLE: sled_screen_title_init(sled); break;
        case SCREEN_EDIT: sled_screen_edit_init(sled); break;
        default: break;
    }
}

void sled_screen_update(SLED *sled, int screen)
{
    switch (screen) {
        case SCREEN_TITLE: sled_screen_title_update(sled); break;
        case SCREEN_EDIT: sled_screen_edit_update(sled); break;
        default: break;
    }
}

void sled_screen_draw(SLED *sled, int screen)
{
    switch (screen) {
        case SCREEN_TITLE: sled_screen_title_draw(sled); break;
        case SCREEN_EDIT: sled_screen_edit_draw(sled); break;
        default: break;
    }
}

void sled_screen_deinit(SLED *sled, int screen)
{
    switch (screen) {
        case SCREEN_TITLE: sled_screen_title_deinit(sled); break;
        case SCREEN_EDIT: sled_screen_edit_deinit(sled); break;
        default: break;
    }
}

void sled_set_current_screen(SLED *sled, int to)
{
    // Deinit the current screen, initialize the one asked for, make the current screen the one asked for
    sled_screen_deinit(sled, sled->screen);
    sled_screen_init(sled, to);
    sled->screen = to;
}

