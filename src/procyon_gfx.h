#ifndef PROCYON_GFX_H
#define PROCYON_GFX_H

#ifdef PAPP_DESKTOP
    #include "glad/glad.h"
#endif

typedef enum pgfx_draw_mode
{
    PAPP_DRAWMODE_NONE,
    PAPP_DRAWMODE_TRIANGLE,
    PAPP_DRAWMODE_LINE
} pgfx_draw_mode;

void pgfx_update_viewport(int width, int height);
bool pgfx_init();
void pgfx_terminate();
void pgfx_render_batch();
void pgfx_begin_drawing(int mode);
void pgfx_end_drawing();
void pgfx_use_texture(GLuint texture_id);
void pgfx_batch_vec2(float x, float y);
void pgfx_batch_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void pgfx_batch_texcoord(float u, float v);
void pgfx_clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

#endif // PROCYON_GFX_H