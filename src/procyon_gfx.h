#ifndef PROCYON_GFX_H
#define PROCYON_GFX_H

#ifdef PAPP_DESKTOP
    #include "glad/glad.h"
#endif

typedef enum pgfx_draw_flags
{
    PGFX_DRAWFLAG_NONE      = 0x0,
    PGFX_DRAWFLAG_TRIANGLES = 0x1,
    PGFX_DRAWFLAG_LINE     = 0x2,
    PGFX_DRAWFLAG_INDEXED   = 0x4,
} pgfx_draw_flags;

void pgfx_update_viewport(int width, int height);
bool pgfx_init();
void pgfx_terminate();
void pgfx_render_batch();
void pgfx_begin_drawing(int flags);
void pgfx_end_drawing();
void pgfx_reserve(int vertex_count, int index_count);
void pgfx_use_texture(GLuint texture_id);
void pgfx_batch_vec2(float x, float y);
void pgfx_batch_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void pgfx_batch_texcoord(float u, float v);
void pgfx_batch_index(unsigned short index);
void pgfx_clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

#endif // PROCYON_GFX_H