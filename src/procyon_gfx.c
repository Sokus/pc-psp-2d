#include "procyon.h"
#include "procyon_gfx.h"

#ifdef PAPP_DESKTOP
    #include "glad/glad.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct pgfx_vertex
{
    papp_vec3 pos;
    papp_color color;
    papp_vec2 texcoord;
} pgfx_vertex;

typedef struct pgfx_draw_call
{
    pgfx_draw_mode mode;
    int vertex_start;
    int vertex_count;
    GLuint texture_id;
} pgfx_draw_call;

typedef struct pgfx_batch
{
    GLuint shader;
    GLuint vertex_buffer, vertex_array;

    float current_z;
    papp_color color;
    papp_vec2 texcoord;

    pgfx_vertex vertices[1024];
    int vertex_count;

    pgfx_draw_call draws[32];
    int draw_count;
} pgfx_batch;
pgfx_batch batch = {0};

void pgfx_uniform_mat4(GLuint program, const char *name, const float* mat4)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, mat4);
}

papp_mat4 papp_ortho(float left, float right, float bottom, float top, float near, float far)
{
    papp_mat4 result = {0.0f};
    result.elements[0][0] = 2.0f / (right - left);
    result.elements[1][1] = 2.0f / (top - bottom);
    result.elements[2][2] = 2.0f / (near - far);
    result.elements[3][3] = 1.0f;

    result.elements[3][0] = (left + right) / (left - right);
    result.elements[3][1] = (bottom + top) / (bottom - top);
    result.elements[3][2] = (far + near) / (near - far);

    return result;
}

void pgfx_update_viewport(int width, int height)
{
    glViewport(0, 0, width, height);
    papp_mat4 ortho = papp_ortho(0, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    pgfx_uniform_mat4(batch.shader, "projection", *ortho.elements);
}

GLuint pgfx_compile_shader(GLenum type, const char *source)
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

GLuint pgfx_create_program(const char *vertex_shader_source, const char *fragment_shader_source)
{
    GLuint vertex_shader_handle = pgfx_compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fragment_shader_handle = pgfx_compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

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

bool pgfx_init()
{
    // shader program
    {
        const char *vertex_shader =
            "#version 420 core                              \n"
            "layout (location = 0) in vec3 aPos;            \n"
            "layout (location = 1) in vec4 aColor;          \n"
            "layout (location = 2) in vec2 aTexCoord;       \n"
            "                                               \n"
            "out vec4 ourColor;                             \n"
            "out vec2 TexCoord;                             \n"
            "                                               \n"
            "uniform mat4 projection;                       \n"
            "                                               \n"
            "void main()                                    \n"
            "{                                              \n"
            "    gl_Position = projection * vec4(aPos, 1.0);\n"
            "    ourColor = aColor;                         \n"
            "    TexCoord = vec2(aTexCoord.x, aTexCoord.y); \n"
            "}";

        const char *fragment_shader =
            "#version 420 core                              \n"
            "out vec4 FragColor;                            \n"
            "                                               \n"
            "in vec4 ourColor;                              \n"
            "in vec2 TexCoord;                              \n"
            "                                               \n"
            "uniform sampler2D texture1;                    \n"
            "                                               \n"
            "void main()                                    \n"
            "{                                              \n"
            "   vec4 texel = texture(texture1, TexCoord);   \n"
            "	FragColor = texel * (ourColor / 255.0);     \n"
            "}";
        batch.shader = pgfx_create_program(vertex_shader, fragment_shader);
    }

    glGenVertexArrays(1, &batch.vertex_array);
    glGenBuffers(1, &batch.vertex_buffer);

    glBindVertexArray(batch.vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, batch.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(batch.vertices), 0, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(pgfx_vertex), (void*)offsetof(pgfx_vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(pgfx_vertex), (void*)offsetof(pgfx_vertex, color));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(pgfx_vertex), (void*)offsetof(pgfx_vertex, texcoord));
    glEnableVertexAttribArray(2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindVertexArray(0);

    batch.draw_count = 1;
    batch.draws[0].vertex_count = 0;
    batch.vertex_count = 0;

    return true;
}

void pgfx_terminate()
{
    glDeleteVertexArrays(1, &batch.vertex_array);
    glDeleteBuffers(1, &batch.vertex_buffer);
}

void pgfx_render_batch()
{
    if(batch.vertex_count > 0)
    {
        glBindVertexArray(batch.vertex_array);

        float *mapped_buffer = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(mapped_buffer, batch.vertices, batch.vertex_count * sizeof(pgfx_vertex));
        glUnmapBuffer(GL_ARRAY_BUFFER);

        glUseProgram(batch.shader);

        for(int draw_idx = 0; draw_idx < batch.draw_count; ++draw_idx)
        {
            pgfx_draw_call *draw_call = batch.draws + draw_idx;
            GLenum mode = 0;
            switch(draw_call->mode)
            {
                case PAPP_DRAWMODE_TRIANGLE: mode = GL_TRIANGLES; break;
                case PAPP_DRAWMODE_LINE: mode = GL_LINE; break;
                default: continue;
            }
            glBindTexture(GL_TEXTURE_2D, draw_call->texture_id);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glDrawArrays(mode, draw_call->vertex_start, draw_call->vertex_count);
        }

        glBindVertexArray(0);
    }

    batch.draw_count = 1;
    batch.draws[0].vertex_count = 0;
    batch.vertex_count = 0;
}

void pgfx_begin_drawing(int mode)
{
    pgfx_draw_call *last_draw_call = &batch.draws[batch.draw_count - 1];
    if(last_draw_call->mode != mode)
    {
        if(last_draw_call->vertex_count > 0)
            batch.draw_count++;

        if(batch.draw_count >= P_ARRAY_COUNT(batch.draws))
            pgfx_render_batch();

        last_draw_call = &batch.draws[batch.draw_count - 1];
        last_draw_call->mode = mode;
        last_draw_call->vertex_count = 0;
        last_draw_call->texture_id = 0; // TODO: set to some common default
    }
}

void pgfx_end_drawing()
{
    batch.current_z += 0.000005f;
}

void pgfx_use_texture(GLuint texture_id)
{
    pgfx_draw_call *last_draw_call = &batch.draws[batch.draw_count - 1];
    if(texture_id != 0 && texture_id != last_draw_call->texture_id)
    {
        if(last_draw_call->vertex_count > 0)
            batch.draw_count++;

        if(batch.draw_count >= P_ARRAY_COUNT(batch.draws))
            pgfx_render_batch();

        last_draw_call = &batch.draws[batch.draw_count - 1];
        last_draw_call->vertex_count = 0;
        last_draw_call->texture_id = texture_id;
    }
}

bool pgfx_would_overflow(int vertex_count)
{
    bool overflow = false;

    if(batch.vertex_count + vertex_count >= P_ARRAY_COUNT(batch.vertices))
    {
        overflow = true;

        pgfx_draw_call *last_draw_call = &batch.draws[batch.draw_count - 1];
        pgfx_draw_mode current_mode = last_draw_call->mode;
        GLuint current_texture_id = last_draw_call->texture_id;

        pgfx_render_batch();
        last_draw_call = &batch.draws[batch.draw_count - 1];
        last_draw_call->mode = current_mode;
        last_draw_call->texture_id = current_texture_id;
    }

    return overflow;
}

void pgfx_batch_vec3(float x, float y, float z)
{
    pgfx_draw_call *last_draw_call = &batch.draws[batch.draw_count - 1];
    int primitive_vertex_count = 0;
    switch(last_draw_call->mode)
    {
        case PAPP_DRAWMODE_TRIANGLE: primitive_vertex_count = 3; break;
        case PAPP_DRAWMODE_LINE:     primitive_vertex_count = 2; break;
        default:                                                 return;
    }

    if(last_draw_call->vertex_count % primitive_vertex_count == 0)
        pgfx_would_overflow(primitive_vertex_count);
    last_draw_call = &batch.draws[batch.draw_count - 1];

    batch.vertices[batch.vertex_count].pos.x = x;
    batch.vertices[batch.vertex_count].pos.y = y;
    batch.vertices[batch.vertex_count].pos.z = z;
    batch.vertices[batch.vertex_count].color = batch.color;
    batch.vertices[batch.vertex_count].texcoord =  batch.texcoord;

    last_draw_call->vertex_count++;
    batch.vertex_count++;
}

void pgfx_batch_vec2(float x, float y)
{
    pgfx_batch_vec3(x, y, batch.current_z);
}

void pgfx_batch_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    batch.color.r = r;
    batch.color.g = g;
    batch.color.b = b;
    batch.color.a = a;
}

void pgfx_batch_texcoord(float u, float v)
{
    batch.texcoord.x = u;
    batch.texcoord.y = v;
}

void pgfx_clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    glClearColor((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    batch.current_z = -1.0f;
}