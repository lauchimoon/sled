#ifndef gfd_h
#define gfd_h

typedef enum {
    DIALOG_OPEN = 0,
    DIALOG_SAVE
} DialogType;

#ifdef __cplusplus
extern "C" {
#endif
int open_file_dialog(const char *, char *, const char *, const char *);
int save_file_dialog(const char *, char *, const char *, const char *);
int file_dialog(int, const char *, char *, const char *, const char *); // General function
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

int save_file_dialog(const char *title, char *filename, const char *filters, const char *message)
{
    int result = -1;

    int filter_count = 0;
    const char **filter_split = TextSplit(filters, ';', &filter_count);
    const char *temp_filename = tinyfd_saveFileDialog(title, filename, filter_count, filter_split, message);

    if (temp_filename != NULL) {
        strcpy(filename, temp_filename);
        result = 1;
    } else {
        result = 0;
    }

    return result;
}

// General function
int file_dialog(int dialog_type, const char *title, char *filename, const char *filters, const char *message)
{
    int result = -1;
    switch (dialog_type) {
        case DIALOG_OPEN: result = open_file_dialog(title, filename, filters, message); break;
        case DIALOG_SAVE: result = save_file_dialog(title, filename, filters, message); break;
        default: break;
    }

    return result;
}

#endif // GFD_IMPLEMENTATION

