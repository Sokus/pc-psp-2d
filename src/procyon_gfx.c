#include "procyon.h"
#include "procyon_gfx.h"

#ifdef PROCYON_DESKTOP
    #include "glad/glad.h"
#endif

#ifdef PROCYON_PSP
    #include <pspdisplay.h>
    #include <pspgu.h>
    #include <pspgum.h>
    #include <pspkernel.h>

    #define PSP_SCR_W 480
    #define PSP_SCR_H 272
    #define PSP_BUF_W 512
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef PROCYON_DESKTOP

typedef struct pgfx_gl_vertex
{
    papp_vec2 pos;
    papp_color color;
    papp_vec2 texcoord;
} pgfx_gl_vertex;

typedef struct pgfx_gl_draw_call
{
    pgfx_draw_flags flags;
    int vertex_data_offset;
    int vertex_count;
    int index_start;
    int index_count;

    GLuint texture_id;
} pgfx_gl_draw_call;

typedef struct pgfx_gl_state
{
    GLuint shader;
    GLuint vertex_buffer, vertex_array, element_buffer;

    unsigned char vertex_data[1024];
    int vertex_data_used;
    unsigned short indices[1024];
    int index_count;
    pgfx_gl_draw_call draws[32];
    int draw_count;

    int vertex_mark;
} pgfx_gl_state;

#endif // PROCYON_DESKTOP

#ifdef PROCYON_PSP

typedef struct pgfx_psp_vertex
{
    papp_vec2 texcoord;
	unsigned int color;
	papp_vec3 pos;
} pgfx_psp_vertex;


typedef struct pgfx_psp_state
{
    unsigned int __attribute__((aligned(16))) list[262144];

    uint8_t __attribute__((aligned(16))) vertex_data[1024];
    int vertex_data_used;
    int vertex_data_start;
    int vertex_count;
    uint16_t __attribute__((aligned(16))) index_data[1024];
    int index_data_used;
    int index_data_start;

    int flags;
} pgfx_psp_state;

#endif // PROCYON_PSP

typedef struct pgfx_state
{
    papp_color color;
    papp_vec2 texcoord;

    #if defined(PROCYON_DESKTOP)
        pgfx_gl_state gl;
    #elif defined(PROCYON_PSP)
        pgfx_psp_state psp;
    #endif
} pgfx_state;
pgfx_state pgfx = {0};

#ifdef PROCYON_DESKTOP

static void pgfx_gl_uniform_mat4(GLuint program, const char *name, const float* mat4)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, mat4);
}

static GLuint pgfx_gl_compile_shader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);

    int compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

    if(compile_status != GL_TRUE)
    {
        int info_log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
        char *info_log = (char *)malloc(info_log_length);
        glGetShaderInfoLog(shader, info_log_length, &info_log_length, info_log);
        const char *shader_type = (type == GL_VERTEX_SHADER ? "Vertex Shader" :
                                   type == GL_FRAGMENT_SHADER ? "Fragment Shader" : "???");
        fprintf(stderr, "GL: %s: %s\n", shader_type, info_log);
        free(info_log);
    }

    return shader;
}

static GLuint pgfx_gl_create_program(const char *vertex_shader_source, const char *fragment_shader_source)
{
    GLuint vertex_shader_handle = pgfx_gl_compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fragment_shader_handle = pgfx_gl_compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

    GLuint program_handle = glCreateProgram();
    glAttachShader(program_handle, vertex_shader_handle);
    glAttachShader(program_handle, fragment_shader_handle);
    glLinkProgram(program_handle);

    int success;
    glGetProgramiv(program_handle, GL_LINK_STATUS, &success);
    if(!success)
    {
        char info_log[512];
        glGetShaderInfoLog(program_handle, 512, 0, info_log);
        fprintf(stderr, "GL: PROGRAM: %s\n", info_log);
    }

    glDetachShader(program_handle, vertex_shader_handle);
    glDeleteShader(vertex_shader_handle);
    glDetachShader(program_handle, fragment_shader_handle);
    glDeleteShader(fragment_shader_handle);

    return program_handle;
}

static void pgfx_gl_render_batch()
{
    if (pgfx.gl.vertex_data_used > 0)
    {
        glBindVertexArray(pgfx.gl.vertex_array);

        float *vertex_buffer = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(vertex_buffer, pgfx.gl.vertex_data, pgfx.gl.vertex_data_used);
        glUnmapBuffer(GL_ARRAY_BUFFER);

        float *index_buffer = (float *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(index_buffer, pgfx.gl.indices, pgfx.gl.index_count * sizeof(unsigned short));
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

        glUseProgram(pgfx.gl.shader);

        for (int draw_idx = 0; draw_idx < pgfx.gl.draw_count; ++draw_idx)
        {
            pgfx_gl_draw_call *draw_call = pgfx.gl.draws + draw_idx;
            GLenum mode;
            switch (draw_call->flags & PGFX_PRIM_BITS)
            {
                case PGFX_PRIM_TRIANGLES: mode = GL_TRIANGLES; break;
                case PGFX_PRIM_LINE: mode = GL_LINE; break;
                default: mode = GL_POINTS; break;
            }

            glBindTexture(GL_TEXTURE_2D, draw_call->texture_id);

            if (draw_call->flags & PGFX_MODE_INDEXED)
            {
                void *index_offset = (void *)(draw_call->index_start * sizeof(GLushort));
                glDrawElements(mode, draw_call->index_count, GL_UNSIGNED_SHORT, index_offset);
            }
            else
            {
                // WARNING: Assumes that we are only using one type of vertex
                //          or that all types are of the same size.
                int vertex_start = draw_call->vertex_data_offset / sizeof(pgfx_gl_vertex);
                glDrawArrays(mode, vertex_start, draw_call->vertex_count);
            }
        }

        glBindVertexArray(0);
    }

    pgfx.gl.draw_count = 1;
    pgfx.gl.vertex_data_used = 0;
    pgfx.gl.index_count = 0;
    pgfx.gl.draws[0].vertex_count = 0;
    pgfx.gl.draws[0].index_count = 0;
}

static bool pgfx_gl_init()
{
    // shader program
    {
        const char *vertex_shader =
            "#version 420 core                                    \n"
            "layout (location = 0) in vec2 aPos;                  \n"
            "layout (location = 1) in vec4 aColor;                \n"
            "layout (location = 2) in vec2 aTexCoord;             \n"
            "                                                     \n"
            "out vec4 ourColor;                                   \n"
            "out vec2 TexCoord;                                   \n"
            "                                                     \n"
            "uniform mat4 projection;                             \n"
            "                                                     \n"
            "void main()                                          \n"
            "{                                                    \n"
            "    gl_Position = projection * vec4(aPos, 0.0, 1.0); \n"
            "    ourColor = aColor;                               \n"
            "    TexCoord = vec2(aTexCoord.x, aTexCoord.y);       \n"
            "}";

        const char *fragment_shader =
            "#version 420 core                                    \n"
            "out vec4 FragColor;                                  \n"
            "                                                     \n"
            "in vec4 ourColor;                                    \n"
            "in vec2 TexCoord;                                    \n"
            "                                                     \n"
            "uniform sampler2D texture1;                          \n"
            "                                                     \n"
            "void main()                                          \n"
            "{                                                    \n"
            "   vec4 texel = texture(texture1, TexCoord);         \n"
            "	FragColor = texel * (ourColor / 255.0);           \n"
            "}";
        pgfx.gl.shader = pgfx_gl_create_program(vertex_shader, fragment_shader);
    }

    glGenVertexArrays(1, &pgfx.gl.vertex_array);
    glGenBuffers(1, &pgfx.gl.vertex_buffer);
    glGenBuffers(1, &pgfx.gl.element_buffer);

    glBindVertexArray(pgfx.gl.vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, pgfx.gl.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pgfx.gl.vertex_data), NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pgfx.gl.element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pgfx.gl.indices), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(pgfx_gl_vertex), (void*)offsetof(pgfx_gl_vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(pgfx_gl_vertex), (void*)offsetof(pgfx_gl_vertex, color));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(pgfx_gl_vertex), (void*)offsetof(pgfx_gl_vertex, texcoord));
    glEnableVertexAttribArray(2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(0);

    pgfx.gl.draw_count = 1;
    pgfx.gl.vertex_data_used = 0;
    pgfx.gl.draws[0].vertex_count = 0;

    return true;
}

static void pgfx_gl_terminate()
{
    glDeleteVertexArrays(1, &pgfx.gl.vertex_array);
    glDeleteBuffers(1, &pgfx.gl.vertex_buffer);
    glDeleteBuffers(1, &pgfx.gl.element_buffer);
}

static void pgfx_gl_start_frame()
{

}

static void pgfx_gl_end_frame()
{
    pgfx_gl_render_batch();
}

static void pgfx_gl_set_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    glClearColor((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f);
}

static void pgfx_gl_clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

static void pgfx_gl_update_viewport(int width, int height)
{
    glViewport(0, 0, width, height);
    papp_mat4 ortho = papp_ortho(0, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    pgfx_gl_uniform_mat4(pgfx.gl.shader, "projection", *ortho.elements);
}

static void pgfx_gl_begin_drawing(int mode)
{
    pgfx_gl_draw_call *last_draw_call = &pgfx.gl.draws[pgfx.gl.draw_count - 1];
    if (last_draw_call->flags != mode)
    {
        GLuint current_texture_id = last_draw_call->texture_id;

        if (last_draw_call->vertex_count > 0)
            pgfx.gl.draw_count++;

        if (pgfx.gl.draw_count >= PROCYON_ARRAY_COUNT(pgfx.gl.draws))
            pgfx_gl_render_batch();

        last_draw_call = &pgfx.gl.draws[pgfx.gl.draw_count - 1];
        last_draw_call->flags = mode;
        last_draw_call->vertex_count = 0;
        last_draw_call->index_count = 0;
        last_draw_call->texture_id = current_texture_id;
    }
}

static void pgfx_gl_end_drawing()
{

}

static void pgfx_gl_reserve(int vertex_data_size, int index_count)
{
    pgfx_gl_draw_call *last_draw_call = &pgfx.gl.draws[pgfx.gl.draw_count - 1];

    if (pgfx.gl.vertex_data_used + vertex_data_size >= PROCYON_ARRAY_COUNT(pgfx.gl.vertex_data) ||
        pgfx.gl.index_count + index_count >= PROCYON_ARRAY_COUNT(pgfx.gl.indices))
    {
        pgfx_draw_flags current_flags = last_draw_call->flags;
        GLuint current_texture_id = last_draw_call->texture_id;

        pgfx_gl_render_batch();
        last_draw_call = &pgfx.gl.draws[pgfx.gl.draw_count - 1];
        last_draw_call->flags = current_flags;
        last_draw_call->texture_id = current_texture_id;
    }

    // WARNING: Assumes that we are only using one type of vertex
    //          or that all types are of the same size.
    pgfx.gl.vertex_mark = pgfx.gl.vertex_data_used / sizeof(pgfx_gl_vertex);

    if (vertex_data_size > 0 && last_draw_call->vertex_count == 0)
        last_draw_call->vertex_data_offset = pgfx.gl.vertex_data_used;

    if (index_count > 0 && last_draw_call->index_count == 0)
        last_draw_call->index_start = pgfx.gl.index_count;
}

static void pgfx_gl_use_texture(papp_texture *texture)
{
    pgfx_gl_draw_call *last_draw_call = &pgfx.gl.draws[pgfx.gl.draw_count - 1];
    if (texture->id != 0 && texture->id != last_draw_call->texture_id)
    {
        pgfx_draw_flags current_flags = last_draw_call->flags;

        if (last_draw_call->vertex_count > 0)
            pgfx.gl.draw_count++;

        if (pgfx.gl.draw_count >= PROCYON_ARRAY_COUNT(pgfx.gl.draws))
            pgfx_gl_render_batch();

        last_draw_call = &pgfx.gl.draws[pgfx.gl.draw_count - 1];
        last_draw_call->flags = current_flags;
        last_draw_call->vertex_count = 0;
        last_draw_call->index_count = 0;
        last_draw_call->texture_id = texture->id;
    }
}

static void pgfx_gl_batch_vec2(float x, float y)
{
    pgfx_gl_vertex *vertex = (pgfx_gl_vertex *)(pgfx.gl.vertex_data + pgfx.gl.vertex_data_used);
    vertex->pos.x = x;
    vertex->pos.y = y;
    vertex->color = pgfx.color;
    vertex->texcoord = pgfx.texcoord;
    pgfx.gl.vertex_data_used += sizeof(pgfx_gl_vertex);

    pgfx.gl.draws[pgfx.gl.draw_count - 1].vertex_count++;
}

void pgfx_gl_batch_index(unsigned short index)
{
    pgfx.gl.indices[pgfx.gl.index_count] = pgfx.gl.vertex_mark + index;
    pgfx.gl.index_count++;
    pgfx.gl.draws[pgfx.gl.draw_count - 1].index_count++;
}

#endif // PROCYON_DESKTOP

#ifdef PROCYON_PSP

static unsigned int pgfx_psp_get_buffer_size(unsigned int width, unsigned int height, unsigned int pixel_format)
{
	switch (pixel_format)
	{
		case GU_PSM_T4:
			return (width * height) >> 1;

		case GU_PSM_T8:
			return width * height;

		case GU_PSM_5650:
		case GU_PSM_5551:
		case GU_PSM_4444:
		case GU_PSM_T16:
			return 2 * width * height;

		case GU_PSM_8888:
		case GU_PSM_T32:
			return 4 * width * height;

		default:
			return 0;
	}
}

static void *pgfx_psp_push_static_buffer(unsigned int width, unsigned int height, unsigned int pixel_format)
{
    static unsigned int static_offset = 0;
    unsigned int size = pgfx_psp_get_buffer_size(width, height, pixel_format);
    void *result = (void *)static_offset;
    static_offset += size;
    return result;
}

void *pgfx_psp_push_static_vram_texture(unsigned int width, unsigned int height, unsigned int pixel_format)
{
    void *vram_buffer_offset = pgfx_psp_push_static_buffer(width, height, pixel_format);
    void *result = (void *)((unsigned int)sceGeEdramGetAddr() + (unsigned int)vram_buffer_offset);
    return result;
}

static void pgfx_psp_render_batch()
{
    sceGuFinish();
    sceGuSync(0, 0);

    pgfx.psp.vertex_data_used = 0;
    pgfx.psp.index_data_used = 0;
    sceKernelDcacheWritebackRange(pgfx.psp.vertex_data, sizeof(pgfx.psp.vertex_data));

    sceGuStart(GU_DIRECT, pgfx.psp.list);
}

static bool pgfx_psp_init()
{
    void* fbp0 = pgfx_psp_push_static_buffer(PSP_BUF_W, PSP_SCR_H, GU_PSM_8888);
	void* fbp1 = pgfx_psp_push_static_buffer(PSP_BUF_W, PSP_SCR_H, GU_PSM_8888);
	void* zbp = pgfx_psp_push_static_buffer(PSP_BUF_W, PSP_SCR_H, GU_PSM_4444);

	sceGuInit();

	sceGuStart(GU_DIRECT, pgfx.psp.list);
	sceGuDrawBuffer(GU_PSM_8888, fbp0, PSP_BUF_W);
	sceGuDispBuffer(PSP_SCR_W, PSP_SCR_H, fbp1, PSP_BUF_W);
	sceGuDepthBuffer(zbp, PSP_BUF_W);
	sceGuOffset(2048 - (PSP_SCR_W/2), 2048 - (PSP_SCR_H/2));
	sceGuViewport(2048, 2048, PSP_SCR_W, PSP_SCR_H);
	sceGuDepthRange(65535, 0);
	sceGuScissor(0, 0, PSP_SCR_W, PSP_SCR_H);
	sceGuEnable(GU_SCISSOR_TEST);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuEnable(GU_BLEND);
	sceGuFrontFace(GU_CCW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumOrtho(0, PSP_SCR_W, PSP_SCR_H, 0, -10.0f, 10.0f);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();

    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();

    return true;
}

static void pgfx_psp_terminate()
{
    sceGuTerm();
}


static void pgfx_psp_start_frame()
{
    sceGuStart(GU_DIRECT, pgfx.psp.list);
}


static void pgfx_psp_end_frame()
{
    pgfx_psp_render_batch();

    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

static void pgfx_psp_set_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    unsigned int color = r | g << 8 | b << 16 | a << 24;
    sceGuClearColor(color);
}

static void pgfx_psp_clear()
{
    sceGuClear(GU_COLOR_BUFFER_BIT | GU_STENCIL_BUFFER_BIT);
}

static void pgfx_psp_update_viewport(int width, int height)
{

}

static void pgfx_psp_begin_drawing(int mode)
{
    pgfx.psp.flags = mode;
    pgfx.psp.vertex_data_start = pgfx.psp.vertex_data_used;
    pgfx.psp.index_data_start = pgfx.psp.index_data_used;
    pgfx.psp.vertex_count = 0;
}

static void pgfx_psp_end_drawing()
{
    int prim;
    switch(pgfx.psp.flags & PGFX_PRIM_BITS)
    {
        case PGFX_PRIM_TRIANGLES: prim = GU_TRIANGLES; break;
        case PGFX_PRIM_LINE: prim = GU_LINES; break;
        case PGFX_PRIM_AABB: prim = GU_SPRITES; break;
        default: prim = GU_POINTS; break;
    }

    int vtype = GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D;
    int count = 0;
    void *indices = 0;
    void *vertices = pgfx.psp.vertex_data + pgfx.psp.vertex_data_start;

    switch(pgfx.psp.flags & PGFX_MODE_BITS)
    {
        case PGFX_MODE_INDEXED:
            vtype |= GU_INDEX_16BIT;
            count = pgfx.psp.index_data_used - pgfx.psp.index_data_start;
            indices = pgfx.psp.index_data + pgfx.psp.index_data_start;
            break;

        default:
            count = pgfx.psp.vertex_count;

    }

    sceGumDrawArray(prim, vtype, count, indices, vertices);
}

static void pgfx_psp_reserve(int vertex_data_size, int index_count)
{
    if(pgfx.psp.vertex_data_used + vertex_data_size > sizeof(pgfx.psp.vertex_data) ||
       pgfx.psp.index_data_used + index_count > PROCYON_ARRAY_COUNT(pgfx.psp.index_data))
    {
        pgfx_psp_render_batch();
    }
}

static void pgfx_psp_use_texture(papp_texture *texture)
{
    sceGuTexMode(GU_PSM_8888, 0, 0, 1);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexWrap(GU_REPEAT, GU_REPEAT);
    sceGuTexImage(0, texture->padded_width, texture->padded_height,
                  texture->padded_width, texture->data);
}

static void pgfx_psp_batch_vec2(float x, float y)
{
    pgfx_psp_vertex *vertex = (pgfx_psp_vertex *)(pgfx.psp.vertex_data + pgfx.psp.vertex_data_used);
    vertex->texcoord = pgfx.texcoord;
    unsigned int color = (pgfx.color.r | pgfx.color.g << 8 | pgfx.color.b << 16 | pgfx.color.a << 24);
    vertex->color = color;
    vertex->pos.x = x;
    vertex->pos.y = y;
    vertex->pos.z = 0.0f;
    pgfx.psp.vertex_data_used += sizeof(pgfx_psp_vertex);
    pgfx.psp.vertex_count++;

}

void pgfx_psp_batch_index(unsigned short index)
{
    pgfx.psp.index_data[pgfx.psp.index_data_used] = index;
    pgfx.psp.index_data_used++;
}

#endif

bool pgfx_init()
{
    #if defined(PROCYON_DESKTOP)
        return pgfx_gl_init();
    #elif defined(PROCYON_PSP)
        return pgfx_psp_init();
    #endif
}

void pgfx_terminate()
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_terminate();
    #elif defined(PROCYON_PSP)
        pgfx_psp_terminate();
    #endif
}

void pgfx_start_frame()
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_start_frame();
    #elif defined(PROCYON_PSP)
        pgfx_psp_start_frame();
    #endif
}

void pgfx_end_frame()
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_end_frame();
    #elif defined(PROCYON_PSP)
        pgfx_psp_end_frame();
    #endif
}

void pgfx_set_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_set_clear_color(r, g, b, a);
    #elif defined(PROCYON_PSP)
        pgfx_psp_set_clear_color(r, g, b, a);
    #endif
}

void pgfx_clear()
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_clear();
    #elif defined(PROCYON_PSP)
        pgfx_psp_clear();
    #endif
}

void pgfx_update_viewport(int width, int height)
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_update_viewport(width, height);
    #elif defined(PROCYON_PSP)
        pgfx_psp_update_viewport(width, height);
    #endif
}

void pgfx_begin_drawing(int mode)
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_begin_drawing(mode);
    #elif defined(PROCYON_PSP)
        pgfx_psp_begin_drawing(mode);
    #endif
}

void pgfx_end_drawing()
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_end_drawing();
    #elif defined(PROCYON_PSP)
        pgfx_psp_end_drawing();
    #endif
}

void pgfx_reserve(int vertex_data_size, int index_count)
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_reserve(vertex_data_size, index_count);
    #elif defined(PROCYON_PSP)
        pgfx_psp_reserve(vertex_data_size, index_count);
    #endif
}

void pgfx_use_texture(papp_texture *texture)
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_use_texture(texture);
    #elif defined(PROCYON_PSP)
        pgfx_psp_use_texture(texture);
    #endif
}

void pgfx_batch_vec2(float x, float y)
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_batch_vec2(x, y);
    #elif defined(PROCYON_PSP)
        pgfx_psp_batch_vec2(x, y);
    #endif
}

void pgfx_batch_index(unsigned short index)
{
    #if defined(PROCYON_DESKTOP)
        pgfx_gl_batch_index(index);
    #elif defined(PROCYON_PSP)
        pgfx_psp_batch_index(index);
    #endif
}

void pgfx_batch_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    pgfx.color.r = r;
    pgfx.color.g = g;
    pgfx.color.b = b;
    pgfx.color.a = a;
}

void pgfx_batch_texcoord(float u, float v)
{
    pgfx.texcoord.x = u;
    pgfx.texcoord.y = v;
}

