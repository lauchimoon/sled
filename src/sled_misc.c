#include "sled_misc.h"
#include <stdio.h>

void draw_text(SLED sled, const char *text, int x, int y, int font_size, Color color)
{
    float spacing = sled.font.baseSize/font_size;
    DrawTextEx(sled.font, text, (Vector2){ (float)x, (float)y }, (float)font_size, spacing, color);
}

void scan_int(const char *text, int *i)
{
    printf("%s", text);
    scanf("%d", i);
}

void scan_str(const char *text, char *s)
{
    printf("%s", text);
    scanf("%s", s);
}

