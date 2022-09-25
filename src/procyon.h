#ifndef PROCYON_H
#define PROCYON_H

#if defined(_WIN32)
    #define PROCYON_DESKTOP
#endif

#if 0
#undef PROCYON_DESKTOP
#define PSP
#endif

#if defined(PSP)
    #define PROCYON_PSP
#endif

#if !defined(PROCYON_DEBUG) && !defined(NDEBUG)
    #define PROCYON_DEBUG
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef enum papp_direction
{
    PAPP_DIRECTION_UP,
    PAPP_DIRECTION_LEFT,
    PAPP_DIRECTION_DOWN,
    PAPP_DIRECTION_RIGHT,
    PAPP_DIRECTION_COUNT
} papp_direction;

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

typedef union papp_color
{
    struct { unsigned char r, g, b, a; };
    unsigned int rgba;
} papp_color;

typedef struct papp_texture {
    int width;
    int height;

    int padded_width;    // Power of two width (PROCYON_PSP only)
    int padded_height;   // Power of two height (PROCYON_PSP only)

    union {
        unsigned int id; // OpenGL texture id (PROCYON_DESKTOP only)
        void *data;      // Texture data (PROCYON_PSP only)
    };
} papp_texture;

typedef struct papp_mat4
{
    float elements[4][4];
} papp_mat4;

#define PROCYON_ARRAY_COUNT(arr) (sizeof(arr)/sizeof((arr)[0]))

#ifdef __cplusplus
extern "C" {
#endif

void papp_init(int width, int height, const char *title);
void papp_terminate();
bool papp_should_close();

void papp_start_frame();
void papp_end_frame();
void papp_set_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void papp_clear();

bool papp_key_down(papp_direction direction);

papp_texture papp_load_texture(const char *path);
void papp_draw_texture(papp_texture texture, float x, float y, float scale);

papp_mat4 papp_ortho(float left, float right, float bottom, float top, float near, float far);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // PROCYON_H