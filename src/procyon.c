#include "procyon.h"
#include "procyon_gfx.h"

#ifdef PAPP_DESKTOP
    #define GLFW_INCLUDE_NONE
    #include "GLFW/glfw3.h"
    #include "glad/glad.h"
#endif // PAPP_DESKTOP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct papp_window
{
    GLFWwindow *handle;
} papp_window;

typedef struct papp_core
{
    const char *title;
    bool should_close;
    int width, height;

    papp_window window;
} papp_core;
papp_core papp = {0};

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

static void papp_glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    papp.width = width;
    papp.height = height;
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
    papp.window.handle = glfwCreateWindow(papp.width, papp.height, window_title, NULL, NULL);

    if(!papp.window.handle) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(papp.window.handle);
    glfwSetKeyCallback(papp.window.handle, papp_glfw_key_callback);
    glfwSetFramebufferSizeCallback(papp.window.handle, papp_glfw_framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    glfwSwapInterval(1);
    return true;
}

void papp_init(int width, int height, const char *title)
{
    papp.width = width;
    papp.height = height;
    papp.title = title;

    if(!papp_glfw_init())
        fprintf(stderr, "Could not init GLFW\n");

    pgfx_init();
}

void papp_terminate()
{
    glfwDestroyWindow(papp.window.handle);
    glfwTerminate();
}

void papp_main()
{
    papp_init(960, 480, "Nie patrz mi sie na tytul");

    papp_texture texture = papp_load_texture("red.png");

    while (!glfwWindowShouldClose(papp.window.handle))
    {
        pgfx_update_viewport(papp.width, papp.height);
        pgfx_clear(46, 34, 47, 255);

        papp_color tint = { 255, 255, 255, 255 };
        papp_draw_texture(texture, 16.0f, 16.0f, 16.0f, tint);

        pgfx_render_batch();

        glfwSwapBuffers(papp.window.handle);
        glfwPollEvents();
    }

    glfwTerminate();
}
