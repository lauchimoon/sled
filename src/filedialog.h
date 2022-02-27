#ifndef gfd_h
#define gfd_h

#ifdef __cplusplus
extern "C" {
#endif
int open_file_dialog(const char *, char *, const char *, const char *);
#ifdef __cplusplus
}
#endif

#endif // gfd_h

#ifdef GFD_IMPLEMENTATION

#include "raylib.h"
#include "tinyfiledialogs.h"

int open_file_dialog(const char *title, char *filename, const char *filters, const char *message)
{
    int result = -1;

    int filter_count = 0;
    const char **filter_split = TextSplit(filters, ';', &filter_count);
    const char *temp_filename = tinyfd_openFileDialog(title, filename, filter_count, filter_split, message, 0);

    if (temp_filename != NULL) {
        strcpy(filename, temp_filename);
        result = 1;
    } else {
        result = 0;
    }

    return result;
}

#endif // GFD_IMPLEMENTATION

