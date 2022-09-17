#ifndef PROCYON_H
#define PROCYON_H

#if defined(_WIN32)
    #define PAPP_DESKTOP
#endif

#if !defined(PROCYON_DEBUG) && !defined(NDEBUG)
    #define PROCYON_DEBUG
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef struct papp_rect
{
    float x, y;
    float width, height;
} papp_rect;

typedef struct papp_vec2
{
    float x, y;
} papp_vec2;

typedef struct papp_vec3
{
    float x, y, z;
} papp_vec3;

typedef struct papp_color
{
    unsigned char r, g, b, a;
} papp_color;

typedef struct papp_texture {
    unsigned int id;
    int width, height, channels;
} papp_texture;

typedef struct papp_mat4
{
    float elements[4][4];
} papp_mat4;

#define P_ARRAY_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

#ifdef __cplusplus
extern "C" {
#endif

void papp_main();

papp_texture papp_load_texture(const char *path);
void papp_draw_texture(papp_texture texture, float x, float y, float scale, papp_color tint);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // PROCYON_H