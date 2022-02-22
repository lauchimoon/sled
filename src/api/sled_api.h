#ifndef sled_api
#define sled_api

typedef struct sled_map {
    unsigned char *grid;
    int width;
    int height;
    int tile_width;
    int tile_height;
    const char *tilesheet;
} sled_map;

#ifdef __cplusplus
extern "C" { // Avoid C++'s name mangling
#endif
sled_map sled_load_map(const char *, const char *);
void sled_free_map(sled_map);
#ifdef __cplusplus
}
#endif

#endif // sled_api
