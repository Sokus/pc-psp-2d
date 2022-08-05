#include "procyon.h"
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef PLATFORM_DESKTOP
    #define GLFW_INCLUDE_NONE
    #include "GLFW/glfw3.h"
    #include "glad/glad.h"
#endif // PLATFORM_DESKTOP

#include "utility.h"

#define PC_UNUSED(arg) (void)(arg)

#define MAX_KEYBOARD_KEYS 512

typedef struct CoreData
{
    struct Window
    {
#ifdef PLATFORM_DESKTOP
        GLFWwindow *handle;
#endif
        const char *title;
        bool ready;
        bool should_close;

        int width;
        int height;
    } window;

    struct Input
    {
        struct Keyboard
        {
            int exit_key;
            char current_key_state[MAX_KEYBOARD_KEYS];
            char previous_key_state[MAX_KEYBOARD_KEYS];
        } keyboard;
    } input;
} CoreData;

CoreData core = {0};

static void pcErrorCallback(int error, const char *description)
{
    PC_UNUSED(error);
    PC_LOG(LOG_ERROR, "GLFW: %s", description);
}

static void pcKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    PC_UNUSED(scancode); PC_UNUSED(mods);
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void pcFramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    PC_UNUSED(window);
    core.window.width = width;
    core.window.height = height;
}

static bool pcInitGraphicsDevice(int width, int height)
{
    glfwSetErrorCallback(pcErrorCallback);

    if(!glfwInit())
    {
        PC_LOG(LOG_FATAL, "GLFW: Failed to initialize GLFW");
        return false;
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const char *glfw_window_title = (core.window.title != NULL) ? core.window.title : " ";
    core.window.handle = glfwCreateWindow(width, height, glfw_window_title, NULL, NULL);

    if(!core.window.handle)
    {
        glfwTerminate();
        PC_LOG(LOG_FATAL, "GLFW: Could not create window");
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
    PC_LOG(LOG_INFO, "Window closed successfully");
}

void pcBeginFrame()
{
    glViewport(0, 0, core.window.width, core.window.height);
    glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void pcEndFrame()
{
    glfwSwapBuffers(core.window.handle);
    glfwPollEvents();
}

bool pcWindowShouldClose()
{
    core.window.should_close = glfwWindowShouldClose(core.window.handle);
    return core.window.should_close;
}