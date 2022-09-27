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
#endif

#include "stb_image.h"

#ifdef PROCYON_DESKTOP

static papp_texture papp_gl_load_texture(const char *path)
{
    stbi_set_flip_vertically_on_load(1);
    int width, height;
    unsigned char *data = stbi_load(path, &width, &height, 0, 4);
    if(data)
    {
        GLuint texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

        papp_texture texture = {0};
        texture.id = texture_id;
        texture.width = width;
        texture.height = height;
        return texture;
    }
    else
    {
        fprintf(stderr, "Could not load texture: %s\n", path);
        papp_texture texture = {0};
        return texture;
    }
}

#endif // PROCYON_DESKTOP

#ifdef PROCYON_PSP

static void papp_psp_swizzle_fast(u8 *out, const u8 *in, const unsigned int width, const unsigned int height)
{
    unsigned int width_blocks = (width / 16);
    unsigned int height_blocks = (height / 8);

    unsigned int src_pitch = (width - 16) / 4;
    unsigned int src_row = width * 8;

    const u8 *ysrc = in;
    u32 *dst = (u32 *)out;

    for (unsigned int blocky = 0; blocky < height_blocks; ++blocky)
    {
        const u8 *xsrc = ysrc;
        for (unsigned int blockx = 0; blockx < width_blocks; ++blockx)
        {
            const u32 *src = (u32 *)xsrc;
            for (unsigned int j = 0; j < 8; ++j)
            {
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                src += src_pitch;
            }
            xsrc += 16;
        }
        ysrc += src_row;
    }
}

static unsigned int papp_psp_closest_greater_pow2(const unsigned int value)
{
    if(value > (1 << 31))
        return 0;
    unsigned int poweroftwo = 1;
    while (poweroftwo < value)
        poweroftwo <<= 1;
    return poweroftwo;
}

static void papp_psp_copy_texture_data(void *dest, const void *src, const int pW, const int width, const int height)
{
    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            ((unsigned int*)dest)[x + y * pW] = ((unsigned int *)src)[x + y * width];
        }
    }
}

static papp_texture papp_psp_load_texture(const char *path)
{
    stbi_set_flip_vertically_on_load(0);
    papp_texture texture = {0};
    int width, height;
    unsigned char *data = stbi_load(path, &width, &height, 0, 4);

    if(data)
    {
        int padded_width = papp_psp_closest_greater_pow2(width);
        int padded_height = papp_psp_closest_greater_pow2(height);
        int size = padded_width * padded_height * 4;

        unsigned int *data_buffer = memalign(16, size);

        if(data_buffer)
        {
            papp_psp_copy_texture_data(data_buffer, data, padded_width, width, height);

            #if 1 // 1 - vram, 0 - ram
                unsigned int *swizzled_pixels = pgfx_psp_push_static_vram_texture(padded_width, padded_height, GU_PSM_8888);
            #else
                unsigned int *swizzled_pixels = memalign(16, size);
            #endif

            if(swizzled_pixels)
            {
                papp_psp_swizzle_fast((u8 *)swizzled_pixels, (const u8 *)data_buffer, padded_width * 4, padded_height);

                texture.data = swizzled_pixels;
                texture.width = width;
                texture.height = height;
                texture.padded_width = padded_width;
                texture.padded_height = padded_height;

                sceKernelDcacheWritebackInvalidateAll();
            }

            free(data_buffer);
        }

        stbi_image_free(data);
    }
    else
    {
        fprintf(stderr, "Could not load texture: %s\n", path);
    }

    return texture;
}

#endif // PROCYON_PSP

papp_texture papp_load_texture(const char *path)
{
    #if defined(PROCYON_DESKTOP)
        return papp_gl_load_texture(path);
    #elif defined(PROCYON_PSP)
        return papp_psp_load_texture(path);
    #endif
}

void papp_draw_texture(papp_texture texture, float x, float y, float scale)
{
    float left = x;
    float right = x + (float)texture.width * scale;
    float bottom = y + (float)texture.height * scale;
    float top = y;

    pgfx_use_texture(&texture);

#if defined(PROCYON_PSP)
    pgfx_batch_color(255, 255, 255, 255);
    pgfx_begin_drawing(0);
        pgfx_batch_texcoord(0.0f, 1.0f); pgfx_batch_vec2(left, top);
        pgfx_batch_texcoord(1.0f, 0.0f); pgfx_batch_vec2(right, bottom);
    pgfx_end_drawing();
#else
    pgfx_begin_drawing(PGFX_PRIM_TRIANGLES | PGFX_MODE_INDEXED);
    pgfx_reserve(4, 6);
        pgfx_batch_color(255, 255, 255, 255);

        pgfx_batch_texcoord(1.0f, 1.0f); pgfx_batch_vec2(right, top);
        pgfx_batch_texcoord(0.0f, 1.0f); pgfx_batch_vec2(left, top);
        pgfx_batch_texcoord(0.0f, 0.0f); pgfx_batch_vec2(left, bottom);
        pgfx_batch_texcoord(1.0f, 0.0f); pgfx_batch_vec2(right, bottom);

        pgfx_batch_index(0); pgfx_batch_index(1); pgfx_batch_index(2);
        pgfx_batch_index(0); pgfx_batch_index(2); pgfx_batch_index(3);
    pgfx_end_drawing();
#endif
}

#define PMATH_PI 3.14159265358979323846
#define PMATH_DEG2RAD (PMATH_PI/180.0f)

#include <math.h>

void papp_draw_texture_pro(papp_texture texture, papp_rect source, papp_rect dest,
                           papp_vec2 origin, float rotation, papp_color tint)
{
    float width = (float)texture.width;
    float height = (float)texture.height;

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
        float sin_rotation = sinf(rotation * PMATH_DEG2RAD);
        float cos_rotation = cosf(rotation * PMATH_DEG2RAD);
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
