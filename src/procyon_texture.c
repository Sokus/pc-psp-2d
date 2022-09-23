#include "procyon.h"
#include "procyon_gfx.h"

#ifdef PROCYON_DESKTOP
    #include "glad/glad.h"
#endif

#ifdef PROCYON_PSP

#endif

#include "stb_image.h"

#include <string.h>

papp_texture papp_load_texture(const char *path)
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
        papp_texture texture;
        memset(&texture, 0, sizeof(papp_texture));
        return texture;
    }
}

void papp_draw_texture(papp_texture texture, float x, float y, float scale)
{
    float left = x;
    float right = x + (float)texture.width * scale;
    float bottom = y + (float)texture.height * scale;
    float top = y;

    pgfx_use_texture(texture.id);

    pgfx_begin_drawing(PGFX_DRAWFLAG_TRIANGLES | PGFX_DRAWFLAG_INDEXED);
    pgfx_reserve(4, 6);
        pgfx_batch_color(255, 255, 255, 255);

        pgfx_batch_texcoord(1.0f, 1.0f); pgfx_batch_vec2(right, top);
        pgfx_batch_texcoord(0.0f, 1.0f); pgfx_batch_vec2(left, top);
        pgfx_batch_texcoord(0.0f, 0.0f); pgfx_batch_vec2(left, bottom);
        pgfx_batch_texcoord(1.0f, 0.0f); pgfx_batch_vec2(right, bottom);

        pgfx_batch_index(0); pgfx_batch_index(1); pgfx_batch_index(2);
        pgfx_batch_index(0); pgfx_batch_index(2); pgfx_batch_index(3);
    pgfx_end_drawing();
}