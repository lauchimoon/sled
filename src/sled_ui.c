#include "sled_ui.h"
#include "sled_misc.h"
#include <string.h>

#define VERYDARKGRAY    (Color){ 32, 32, 32, 255 }

bool sled_ui_button_hovered(SledUIButton button)
{
    return (CheckCollisionPointRec(GetMousePosition(), button.bounds));
}

bool sled_ui_button_pressed(SledUIButton button)
{
    if (sled_ui_button_hovered(button)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return true;
        }
    }

    return false;
}

void sled_ui_button_draw(SLED sled, SledUIButton button)
{
    Color outline_color = (sled_ui_button_hovered(button))? DARKBLUE : VERYDARKGRAY;
    Color inside_color = (sled_ui_button_hovered(button))? SKYBLUE : GRAY;

    DrawRectangleRec(button.bounds, inside_color);
    DrawRectangleLinesEx(button.bounds, 5, outline_color);

    float font_size = 24.0f;
    float spacing = sled.font.baseSize/font_size;
    Vector2 size = MeasureTextEx(sled.font, button.text, font_size, spacing);
    Vector2 pos = { (button.bounds.width/2 - size.x/2) + 10.0f, (button.bounds.y + button.bounds.height/2) - 10.0f };

    draw_text(sled, button.text, (int)pos.x, (int)pos.y, (int)font_size, outline_color);
}

void sled_ui_textbox(SLED sled, SledUITextBox *tb, char content[], int max, bool numeric)
{
    // Update
    tb->over = (CheckCollisionPointRec(GetMousePosition(), tb->bounds));

    if (tb->over) {
        int key = GetCharPressed();
        bool cond;

        cond = numeric? (key >= 48) && (key <= 58) : (key >= 32) && (key <= 126);

        while (key > 0) {
            if (cond && (tb->letter_count < max)) {
                content[tb->letter_count] = (char)key;
                content[tb->letter_count + 1] = '\0';
                tb->letter_count++;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            tb->letter_count--;
            if (tb->letter_count < 0) {
                tb->letter_count = 0;
            }
            content[tb->letter_count] = '\0';
        }
    }

    // Draw
    int len = strlen(content);
    Color outline_color = (tb->over)? DARKBLUE : VERYDARKGRAY;
    Color inside_color = (tb->over)? SKYBLUE : WHITE;
    DrawRectangleRec(tb->bounds, inside_color);
    DrawRectangleLinesEx(tb->bounds, 5, len < 1? MAROON : outline_color);

    float font_size = 20.0f;
    draw_text(sled, len < 1? tb->empty_text : content, (int)tb->bounds.x + 10, (int)tb->bounds.y + 12, (int)font_size, len < 1? LIGHTGRAY : outline_color);
}

