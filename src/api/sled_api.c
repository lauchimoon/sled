#include "sled_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT_BUFFER_LENGTH  2048
#define SPLIT_COUNT_MAX     128

static char *read_to_string(const char *);
static char **split_string(const char *, char);
static unsigned char *load_file_data(const char *);

static char *info_str = 0;

sled_map sled_load_map(const char *map_file, const char *map_info_file)
{
    sled_map map = { 0 };
    info_str = read_to_string(map_info_file);
    char **info_split = split_string(info_str, '\n');

    map.grid = load_file_data(map_file);
    map.width = atoi(info_split[0]);
    map.height = atoi(info_split[1]);
    map.tile_width = atoi(info_split[2]);
    map.tile_height = atoi(info_split[3]);
    map.tilesheet = info_split[4];

    return map;
}

void sled_free_map(sled_map map)
{
    if (map.grid) {
        free(map.grid);
    }
 
    free(info_str);
}

// ----------

static char *read_to_string(const char *file)
{
    char *buffer = 0;

    FILE *f = fopen(file, "rb");

    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        int size = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (size > 0) {
            buffer = malloc(size);
            fread(buffer, sizeof(char), size, f);
        }

        fclose(f);
    }

    return buffer;
}

// https://github.com/raysan5/raylib/blob/master/src/rtext.c#L1476
static char **split_string(const char *str, char delimiter)
{
    static char *result[SPLIT_COUNT_MAX] = { NULL };
    static char buffer[TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, TEXT_BUFFER_LENGTH);

    result[0] = buffer;
    int counter = 0;

    if (str != NULL) {
        counter = 1;

        // Count the amount of substrings and point to each one
        for (int i = 0; i < TEXT_BUFFER_LENGTH; i++) {
            buffer[i] = str[i];
            if (buffer[i] == '\0') {
                break;
            } else if (buffer[i] == delimiter) {
                buffer[i] = '\0';
                result[counter] = buffer + i + 1;
                counter++;

                if (counter == SPLIT_COUNT_MAX) {
                    break;
                }
            }
        }
    }

    return result;
}

static unsigned char *load_file_data(const char *file)
{
    unsigned char *data = NULL;

    FILE *f = fopen(file, "rb");
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        int size = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (size > 0) {
            data = malloc(size*sizeof(unsigned char));
            fread(data, sizeof(unsigned char), size, f);
        }

        fclose(f);
    }

    return data;
}

