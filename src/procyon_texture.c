#include "procyon.h"
#include "procyon_gfx.h"

#ifdef PROCYON_DESKTOP
    #include "glad/glad.h"
#endif

#ifdef PROCYON_PSP
    #include <pspkernel.h>
    #include <pspgu.h>
    #include <pspgum.h>
    #include <malloc.h>
    #include <string.h>

    #define PSP_SCR_W 480
    #define PSP_SCR_H 272
    #define PSP_BUF_W 512
#endif

#include "stb_image.h"

#include <math.h>

papp_texture papp_load_texture(const char *path)
{
    stbi_set_flip_vertically_on_load(0);
    int width, height;
    unsigned char *data = stbi_load(path, &width, &height, 0, 4);

    papp_texture texture = {0};

    if(data)
    {
        texture = pgfx_create_texture(data, width, height, true);
        stbi_image_free(data);
    }
    else
    {
        fprintf(stderr, "Could not load texture: %s\n", path);
    }

    return texture;
}

void papp_draw_texture(papp_texture texture, float x, float y)
{
    papp_rect source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    papp_rect dest = { x, y, (float)texture.width, (float)texture.height };
    papp_vec2 origin = { 0.0f, 0.0f };
    papp_color tint = { 255, 255, 255, 255 };
    papp_draw_texture_ex(texture, source, dest, origin, 0.0f, tint);

}

void papp_draw_texture_rect(papp_texture texture, papp_rect source, papp_rect dest)
{
    papp_vec2 origin = { 0.0f, 0.0f };
    papp_color tint = { 255, 255, 255, 255 };
    papp_draw_texture_ex(texture, source, dest, origin, 0.0f, tint);
}

void papp_draw_texture_ex(papp_texture texture, papp_rect source, papp_rect dest,
                           papp_vec2 origin, float rotation, papp_color tint)
{
    #if defined(PROCYON_PSP)
        float width = (float)texture.padded_width;
        float height = (float)texture.padded_height;
    #else
        float width = (float)texture.width;
        float height = (float)texture.height;
    #endif

    bool flip_x = false;

    if (source.width < 0) { flip_x = true; source.width *= -1; }
    if (source.height < 0) source.y -= source.height;

    papp_vec2 top_left_uv, top_right_uv, bottom_left_uv, bottom_right_uv;

    if(flip_x)
    {
        top_left_uv = (papp_vec2){ (source.x + source.width)/width, source.y/height };
        top_right_uv = (papp_vec2) { source.x/width, source.y/height };
        bottom_left_uv = (papp_vec2) { (source.x + source.width)/width, (source.y + source.height)/height };
        bottom_right_uv = (papp_vec2) { source.x/width, (source.y + source.height)/height };
    }
    else
    {
        top_left_uv = (papp_vec2){ source.x/width, source.y/height };
        top_right_uv = (papp_vec2) { (source.x + source.width)/width, source.y/height };
        bottom_left_uv= (papp_vec2) { source.x/width, (source.y + source.height)/height };
        bottom_right_uv = (papp_vec2) { (source.x + source.width)/width, (source.y + source.height)/height };
    }

    papp_vec2 top_left = { 0.0f };
    papp_vec2 top_right = { 0.0f };
    papp_vec2 bottom_left = { 0.0f };
    papp_vec2 bottom_right = { 0.0f };

    if(rotation == 0.0f)
    {
        float x = dest.x - origin.x;
        float y = dest.y - origin.y;

        top_left = (papp_vec2){ x, y };
        top_right = (papp_vec2){ x + dest.width, y };
        bottom_left = (papp_vec2){ x, y + dest.height };
        bottom_right = (papp_vec2){ x + dest.width, y + dest.height };
    }
    else
    {
        const float pi = 3.14159265358979323846f;
        const float deg_to_rad = pi / 180.0f;

        float sin_rotation = sinf(rotation * deg_to_rad);
        float cos_rotation = cosf(rotation * deg_to_rad);
        float x = dest.x;
        float y = dest.y;
        float dx = -origin.x;
        float dy = -origin.y;

        top_left.x = x + dx*cos_rotation - dy*sin_rotation;
        top_left.y = y + dx*sin_rotation + dy*cos_rotation;

        top_right.x = x + (dx + dest.width)*cos_rotation - dy*sin_rotation;
        top_right.y = y + (dx + dest.width)*sin_rotation + dy*cos_rotation;

        bottom_left.x = x + dx*cos_rotation - (dy + dest.height)*sin_rotation;
        bottom_left.y = y + dx*sin_rotation + (dy + dest.height)*cos_rotation;

        bottom_right.x = x + (dx + dest.width)*cos_rotation - (dy + dest.height)*sin_rotation;
        bottom_right.y = y + (dx + dest.width)*sin_rotation + (dy + dest.height)*cos_rotation;
    }

    #if defined(PROCYON_PSP)
        bool do_aabb = (rotation == 0.0f);
    #else
        bool do_aabb = false;
    #endif

    pgfx_use_texture(&texture);
    pgfx_batch_color(255, 255, 255, 255);

    if(do_aabb)
    {
        pgfx_begin_drawing(PGFX_PRIM_AABB);
        pgfx_reserve(2, 0);
            pgfx_batch_texcoord(top_left_uv.x, top_left_uv.y);
            pgfx_batch_vec2(top_left.x, top_left.y);

            pgfx_batch_texcoord(bottom_right_uv.x, bottom_right_uv.y);
            pgfx_batch_vec2(bottom_right.x, bottom_right.y);
        pgfx_end_drawing();
    }
    else
    {
        pgfx_begin_drawing(PGFX_PRIM_TRIANGLES | PGFX_MODE_INDEXED);
        pgfx_reserve(4, 6);
            pgfx_batch_texcoord(bottom_left_uv.x, bottom_left_uv.y);
            pgfx_batch_vec2(bottom_left.x, bottom_left.y);

            pgfx_batch_texcoord(top_right_uv.x, top_right_uv.y);
            pgfx_batch_vec2(top_right.x, top_right.y);

            pgfx_batch_texcoord(top_left_uv.x, top_left_uv.y);
            pgfx_batch_vec2(top_left.x, top_left.y);

            pgfx_batch_texcoord(bottom_right_uv.x, bottom_right_uv.y);
            pgfx_batch_vec2(bottom_right.x, bottom_right.y);

            pgfx_batch_index(0); pgfx_batch_index(1); pgfx_batch_index(2);
            pgfx_batch_index(1); pgfx_batch_index(0); pgfx_batch_index(3);
        pgfx_end_drawing();
    }
}

papp_render_target papp_create_render_target(int width, int height)
{
    papp_render_target render_target = {0};

    #if defined(PROCYON_DESKTOP)
        render_target.texture = pgfx_create_texture(0, width, height, false);

        glGenFramebuffers(1, &render_target.id);
        glBindFramebuffer(GL_FRAMEBUFFER, render_target.id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_target.texture.id, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    #elif defined(PROCYON_PSP)
        render_target.texture = pgfx_create_texture(0, width, height, false);
        render_target.edram_offset = render_target.texture.data - (int)sceGeEdramGetAddr();
    #endif

    return render_target;
}