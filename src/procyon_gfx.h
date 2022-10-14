#ifndef PROCYON_GFX_H
#define PROCYON_GFX_H

#include "procyon.h"

#ifdef PROCYON_DESKTOP
    #include "glad/glad.h"
#endif

typedef enum pgfx_draw_flags
{
    PGFX_PRIM_TRIANGLES = 0x1, // 0000 0001
    PGFX_PRIM_LINE      = 0x2, // 0000 0010
    PGFX_PRIM_AABB      = 0x3, // 0000 0011
    PGFX_PRIM_BITS      = 0x3, // 0000 0011

    PGFX_MODE_INDEXED   = 0x4, // 0000 0100
    PGFX_MODE_BITS      = 0x4, // 0000 0100
} pgfx_draw_flags;

#if defined(PROCYON_DESKTOP)
    #define pgfx_enable glEnable
    #define pgfx_disable glDisable
    #define pgfx_front_face glFrontFace
    #define PGFX_CULL_FACE GL_CULL_FACE
    #define PGFX_CW        GL_CW
    #define PGFX_CCW       GL_CCW
#endif

#if defined(PROCYON_PSP)
    #define pgfx_enable sceGuEnable
    #define pgfx_disable sceGuDisable
    #define pgfx_front_face sceGuFrontFace
    #define PGFX_CULL_FACE GU_CULL_FACE
    #define PGFX_CW        GU_CW
    #define PGFX_CCW       GU_CCW
#endif

bool pgfx_init();
void pgfx_terminate();
void pgfx_start_frame();
void pgfx_end_frame();
void pgfx_render_batch();
void pgfx_set_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void pgfx_clear();

void pgfx_bind_render_target(papp_render_target *render_target);
void pgfx_unbind_render_target();
void pgfx_ortho(float left, float right, float bottom, float top, float near, float far);
void pgfx_update_viewport(int width, int height);

void pgfx_begin_drawing(int flags);
void pgfx_end_drawing();
void pgfx_reserve(int vertex_count, int index_count);
void pgfx_use_texture(papp_texture *texture);
void pgfx_batch_vec2(float x, float y);
void pgfx_batch_index(unsigned short index);
void pgfx_batch_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void pgfx_batch_texcoord(float u, float v);

papp_texture pgfx_create_texture(void *data, int width, int height, bool swizzle);

#endif // PROCYON_GFX_H