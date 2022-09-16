#include "procyon.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef PAPP_DESKTOP
    #define GLFW_INCLUDE_NONE
    #include "GLFW/glfw3.h"
    #include "glad/glad.h"
#endif // PAPP_DESKTOP

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct papp_glfw
{
    GLFWwindow *window;
} papp_glfw;

typedef struct papp_rect
{
    float x, y;
    float width, height;
} papp_rect;

typedef struct papp_vec3
{
    float x, y, z;
} papp_vec3;

typedef struct papp_vec2
{
    float x, y;
} papp_vec2;

typedef struct papp_color
{
    unsigned char r, g, b, a;
} papp_color;

typedef struct papp_vertex
{
    papp_vec3 pos;
    papp_color color;
    papp_vec2 texcoord;
} papp_vertex;

typedef enum papp_draw_mode
{
    PAPP_DRAWMODE_NONE,
    PAPP_DRAWMODE_TRIANGLE,
    PAPP_DRAWMODE_LINE
} papp_draw_mode;

typedef struct papp_draw_call
{
    papp_draw_mode mode;
    int vertex_start;
    int vertex_count;
    GLuint texture_id;
} papp_draw_call;

typedef struct papp_batch
{
    GLuint shader;
    GLuint vertex_buffer, vertex_array;

    papp_color color;
    papp_vec2 texcoord;

    papp_vertex vertices[1024];
    int vertex_count;

    papp_draw_call draws[32];
    int draw_count;
} papp_batch;

typedef struct papp_core
{
    const char *title;
    bool should_close;
    int width, height;

    papp_glfw glfw;
    papp_batch batch;
} papp_core;
papp_core papp = {0};

typedef struct papp_texture {
    GLuint id;
    int width, height, channels;
} papp_texture;

const char *papp_gl_GLenum_to_string(GLenum value)
{
    #define STRINGIFY_CASE(enum_value) case enum_value: return #enum_value
    switch(value)
    {
        STRINGIFY_CASE(GL_VERTEX_SHADER);
        STRINGIFY_CASE(GL_FRAGMENT_SHADER);
        default: return "UNKNOWN TYPE";
    }
    #undef STRINGIFY_CASE
}

GLuint papp_gl_compile_shader(GLenum type, const char *source)
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
        fprintf(stderr, "GL: %s: %s\n", papp_gl_GLenum_to_string(type), info_log);
        free(info_log);
    }

    return shader;
}

GLuint papp_gl_create_program(const char *vertex_shader_source, const char *fragment_shader_source)
{
    GLuint vertex_shader_handle = papp_gl_compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fragment_shader_handle = papp_gl_compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

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

static void papp_glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW: %s\n", description);
}

static void papp_glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static bool papp_glfw_init()
{
    glfwSetErrorCallback(papp_glfw_error_callback);

    if(!glfwInit())
        return false;

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const char *window_title = (papp.title != NULL && papp.title[0] != '\0') ? papp.title : " ";
    papp.glfw.window = glfwCreateWindow(papp.width, papp.height, window_title, NULL, NULL);

    if(!papp.glfw.window) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(papp.glfw.window);
    glfwSetKeyCallback(papp.glfw.window, papp_glfw_key_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    glfwSwapInterval(1);
    return true;
}

static bool papp_gl_init()
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
        papp.batch.shader = papp_gl_create_program(vertex_shader, fragment_shader);
    }

    glGenVertexArrays(1, &papp.batch.vertex_array);
    glGenBuffers(1, &papp.batch.vertex_buffer);

    glBindVertexArray(papp.batch.vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, papp.batch.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(papp.batch.vertices), 0, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(papp_vertex), (void*)offsetof(papp_vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(papp_vertex), (void*)offsetof(papp_vertex, color));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(papp_vertex), (void*)offsetof(papp_vertex, texcoord));
    glEnableVertexAttribArray(2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(0);

    return true;
}

typedef union papp_mat4
{
    float elements[4][4];
    float arr[16];
} papp_mat4;

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

void papp_set_mat4_uniform(GLuint program, const char *name, const float* mat4)
{
    GLint uniform_location = glGetUniformLocation(program, name);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, mat4);
}

void papp_update_viewport()
{
    glfwGetFramebufferSize(papp.glfw.window, &papp.width, &papp.height);
    glViewport(0, 0, papp.width, papp.height);
    papp_mat4 ortho = papp_ortho(0, (float)papp.width, (float)papp.height, 0, -1.0f, 1.0f);
    papp_set_mat4_uniform(papp.batch.shader, "projection", ortho.arr);
}

void papp_init(int width, int height, const char *title)
{
    papp.width = width;
    papp.height = height;
    papp.title = title;

    if(!papp_glfw_init())
        fprintf(stderr, "Could not init GLFW\n");

    papp_gl_init();
    papp_update_viewport();
}

void papp_terminate()
{
    glfwDestroyWindow(papp.glfw.window);
    glfwTerminate();
}

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

void papp_render_batch()
{
    if(papp.batch.vertex_count > 0)
    {
        glBindVertexArray(papp.batch.vertex_array);

        float *mapped_buffer = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(mapped_buffer, papp.batch.vertices, papp.batch.vertex_count * sizeof(papp_vertex));
        glUnmapBuffer(GL_ARRAY_BUFFER);

        glUseProgram(papp.batch.shader);

        for(int draw_idx = 0; draw_idx < papp.batch.draw_count; ++draw_idx)
        {
            papp_draw_call *draw_call = papp.batch.draws + draw_idx;
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

    papp.batch.draw_count = 1;
    papp.batch.draws[0].vertex_count = 0;
    papp.batch.vertex_count = 0;
}

#define P_ARRAY_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

void papp_begin_drawing(int mode)
{
    papp_draw_call *last_draw_call = &papp.batch.draws[papp.batch.draw_count - 1];
    if(last_draw_call->mode != mode)
    {
        if(last_draw_call->vertex_count > 0)
            papp.batch.draw_count++;

        if(papp.batch.draw_count >= P_ARRAY_COUNT(papp.batch.draws))
            papp_render_batch();

        last_draw_call = &papp.batch.draws[papp.batch.draw_count - 1];
        last_draw_call->mode = mode;
        last_draw_call->vertex_count = 0;
        last_draw_call->texture_id = 0; // TODO: set to some common default
    }
}

void papp_set_texture(GLuint texture_id)
{
    papp_draw_call *last_draw_call = &papp.batch.draws[papp.batch.draw_count - 1];
    if(texture_id != 0 && texture_id != last_draw_call->texture_id)
    {
        if(last_draw_call->vertex_count > 0)
            papp.batch.draw_count++;

        if(papp.batch.draw_count >= P_ARRAY_COUNT(papp.batch.draws))
            papp_render_batch();

        last_draw_call = &papp.batch.draws[papp.batch.draw_count - 1];
        last_draw_call->vertex_count = 0;
        last_draw_call->texture_id = texture_id;
    }
}

bool papp_batch_would_overflow(int vertex_count)
{
    bool overflow = false;

    if(papp.batch.vertex_count + vertex_count >= P_ARRAY_COUNT(papp.batch.vertices))
    {
        overflow = true;

        papp_draw_call *last_draw_call = &papp.batch.draws[papp.batch.draw_count - 1];
        papp_draw_mode current_mode = last_draw_call->mode;
        GLuint current_texture_id = last_draw_call->texture_id;

        papp_render_batch();
        last_draw_call = &papp.batch.draws[papp.batch.draw_count - 1];
        last_draw_call->mode = current_mode;
        last_draw_call->texture_id = current_texture_id;
    }

    return overflow;
}

void papp_batch_vec3(float x, float y, float z)
{
    papp_draw_call *last_draw_call = &papp.batch.draws[papp.batch.draw_count - 1];
    int primitive_vertex_count = 0;
    switch(last_draw_call->mode)
    {
        case PAPP_DRAWMODE_TRIANGLE: primitive_vertex_count = 3; break;
        case PAPP_DRAWMODE_LINE:     primitive_vertex_count = 2; break;
        default:                                                 return;
    }

    if(last_draw_call->vertex_count % primitive_vertex_count == 0)
        papp_batch_would_overflow(primitive_vertex_count);
    last_draw_call = &papp.batch.draws[papp.batch.draw_count - 1];

    papp.batch.vertices[papp.batch.vertex_count].pos.x = x;
    papp.batch.vertices[papp.batch.vertex_count].pos.y = y;
    papp.batch.vertices[papp.batch.vertex_count].pos.z = z;
    papp.batch.vertices[papp.batch.vertex_count].color = papp.batch.color;
    papp.batch.vertices[papp.batch.vertex_count].texcoord =  papp.batch.texcoord;

    last_draw_call->vertex_count++;
    papp.batch.vertex_count++;
}

void papp_batch_vec2(float x, float y)
{
    papp_batch_vec3(x, y, 0.0f);
}

void papp_batch_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    papp.batch.color.r = r;
    papp.batch.color.g = g;
    papp.batch.color.b = b;
    papp.batch.color.a = a;
}

void papp_batch_texcoord(float u, float v)
{
    papp.batch.texcoord.x = u;
    papp.batch.texcoord.y = v;
}

void papp_draw_texture(papp_texture texture, float x, float y, float scale)
{
    papp_set_texture(texture.id);
    papp_batch_color(255, 255, 255, 255);

    float left = x;
    float right = x + (float)texture.width * scale;
    float bottom = y;
    float top = y + (float)texture.height * scale;

    papp_begin_drawing(PAPP_DRAWMODE_TRIANGLE);
        papp_batch_texcoord(1.0f, 1.0f); papp_batch_vec2(   top, right);
        papp_batch_texcoord(1.0f, 0.0f); papp_batch_vec2(   top,  left);
        papp_batch_texcoord(0.0f, 0.0f); papp_batch_vec2(bottom,  left);

        papp_batch_texcoord(1.0f, 1.0f); papp_batch_vec2(   top, right);
        papp_batch_texcoord(0.0f, 0.0f); papp_batch_vec2(bottom,  left);
        papp_batch_texcoord(0.0f, 1.0f); papp_batch_vec2(bottom, right);
}

void papp_main()
{
    papp_init(960, 480, "Nie patrz mi sie na tytul");

    papp_texture texture = papp_load_texture("red.png");

    while (!glfwWindowShouldClose(papp.glfw.window))
    {
        papp_update_viewport();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        papp_draw_texture(texture, 0.0f, 0.0f, 4.0f);

        papp_render_batch();

        glfwSwapBuffers(papp.glfw.window);
        glfwPollEvents();
        papp_update_viewport();
    }

    glDeleteVertexArrays(1, &papp.batch.vertex_array);
    glDeleteBuffers(1, &papp.batch.vertex_buffer);

    glfwTerminate();
}
