#ifndef sled_screens_h
#define sled_screens_h

#include "sled_context.h"

typedef enum {
    SCREEN_TITLE = 0,
    SCREEN_EDIT
} Screens;

void sled_screen_init(SLED *, int);
void sled_screen_update(SLED *, int);
void sled_screen_draw(SLED *, int);
void sled_screen_deinit(SLED *, int);
void sled_set_current_screen(SLED *, int);

void sled_screen_title_init(SLED *);
void sled_screen_title_update(SLED *);
void sled_screen_title_draw(SLED *);
void sled_screen_title_deinit(SLED *);

void sled_screen_edit_init(SLED *);
void sled_screen_edit_update(SLED *);
void sled_screen_edit_draw(SLED *);
void sled_screen_edit_deinit(SLED *);

#endif // sled_screens_h
