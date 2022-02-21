#ifndef sled_misc_h
#define sled_misc_h

#include "sled_context.h"

void draw_text(SLED, const char *, int, int, int, Color);

void scan_int(const char *, int *);
void scan_str(const char *, char *);

#endif // sled_misc_h
