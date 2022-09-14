#include "procyon.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef PLATFORM_DESKTOP
    #define GLFW_INCLUDE_NONE
    #include "GLFW/glfw3.h"
    #include "glad/glad.h"
#endif // PLATFORM_DESKTOP

typedef struct CoreData
{
    struct Window
    {
        GLFWwindow *handle;
        const char *title;
        bool ready;
        bool should_close;

        int width;
        int height;
    } window;
} CoreData;
CoreData core = {0};

static void pcErrorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW: %s\n", description);
}

static void pcKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void pcFramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    core.window.width = width;
    core.window.height = height;
}

static bool pcInitGraphicsDevice(int width, int height)
{
    glfwSetErrorCallback(pcErrorCallback);

    if(!glfwInit())
        return false;

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const char *glfw_window_title = (core.window.title != NULL) ? core.window.title : " ";
    core.window.handle = glfwCreateWindow(width, height, glfw_window_title, NULL, NULL);

    if(!core.window.handle)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(core.window.handle);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);

    glfwSetKeyCallback(core.window.handle, pcKeyCallback);
    glfwSetFramebufferSizeCallback(core.window.handle, pcFramebufferSizeCallback);

    return true;
}

void pcInitWindow(int width, int height, const char *title)
{
    core.window.width = width;
    core.window.height = height;

    if(title != NULL && title[0] != 0)
        core.window.title = title;

    core.window.ready = pcInitGraphicsDevice(width, height);
}

void pcCloseWindow()
{
    glfwDestroyWindow(core.window.handle);
    core.window.ready = false;
    glfwTerminate();
}

void pcBeginFrame()
{
    glfwPollEvents();
    glViewport(0, 0, core.window.width, core.window.height);
    glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void pcEndFrame()
{
    glfwSwapBuffers(core.window.handle);
}

bool pcWindowShouldClose()
{
    core.window.should_close = glfwWindowShouldClose(core.window.handle);
    return core.window.should_close;
}