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

bool pgfx_init();
void pgfx_terminate();
void pgfx_start_frame();
void pgfx_end_frame();
void pgfx_set_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void pgfx_clear();

void pgfx_bind_render_target(papp_render_target *render_target);
void pgfx_unbind_render_target();
void pgfx_update_viewport(int width, int height);

void pgfx_begin_drawing(int flags);
void pgfx_end_drawing();
void pgfx_reserve(int vertex_count, int index_count);
void pgfx_use_texture(papp_texture *texture);
void pgfx_batch_vec2(float x, float y);
void pgfx_batch_index(unsigned short index);
void pgfx_batch_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void pgfx_batch_texcoord(float u, float v);

unsigned int pgfx_psp_get_buffer_size(unsigned int width, unsigned int height, unsigned int pixel_format);
void *pgfx_psp_edram_push_size(unsigned int size);

papp_texture pgfx_create_texture(void *data, int width, int height, bool swizzle);

#endif // PROCYON_GFX_H