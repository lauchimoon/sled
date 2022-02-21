#ifndef sled_ui_h
#define sled_ui_h

#include "sled_context.h"

typedef struct SledUIButton {
    Rectangle bounds;
    const char *text;
} SledUIButton;

typedef struct SledUITextBox {
    Rectangle bounds;
    bool over;
    int letter_count;
    char *empty_text;
} SledUITextBox;

bool sled_ui_button_hovered(SledUIButton);
bool sled_ui_button_pressed(SledUIButton);
void sled_ui_button_draw(SLED, SledUIButton);
void sled_ui_textbox(SLED, SledUITextBox *, char [], int, bool);

#endif // sled_ui_h
