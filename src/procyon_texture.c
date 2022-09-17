#include "procyon.h"
#include "procyon_gfx.h"

#ifdef PAPP_DESKTOP
    #include "glad/glad.h"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

papp_texture papp_load_texture(const char *path)
{
    stbi_set_flip_vertically_on_load(1);
    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
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

        papp_texture texture;
        texture.id = texture_id;
        texture.width = width;
        texture.height = height;
        texture.channels = channels;
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

void papp_draw_texture(papp_texture texture, float x, float y, float scale, papp_color tint)
{
    pgfx_use_texture(texture.id);
    pgfx_batch_color(tint.r, tint.g, tint.b, tint.a);

    float left = x;
    float right = x + (float)texture.width * scale;
    float bottom = y;
    float top = y + (float)texture.height * scale;

    pgfx_begin_drawing(PAPP_DRAWMODE_TRIANGLE);
        pgfx_batch_texcoord(1.0f, 0.0f); pgfx_batch_vec2(   top, right);
        pgfx_batch_texcoord(1.0f, 1.0f); pgfx_batch_vec2(   top,  left);
        pgfx_batch_texcoord(0.0f, 1.0f); pgfx_batch_vec2(bottom,  left);

        pgfx_batch_texcoord(1.0f, 0.0f); pgfx_batch_vec2(   top, right);
        pgfx_batch_texcoord(0.0f, 1.0f); pgfx_batch_vec2(bottom,  left);
        pgfx_batch_texcoord(0.0f, 0.0f); pgfx_batch_vec2(bottom, right);
    pgfx_end_drawing();
}