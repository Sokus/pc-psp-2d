#include "procyon.h"
#include "procyon_gfx.h"

#ifdef PROCYON_DESKTOP
    #define GLFW_INCLUDE_NONE
    #include "GLFW/glfw3.h"
    #include "glad/glad.h"
#endif // PROCYON_DESKTOP

#ifdef PROCYON_PSP
    #include <pspkernel.h>
    #include <pspctrl.h>

    PSP_MODULE_INFO("Procyon", 0, 1, 1);
    PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

    #define PSP_SCR_W 480
    #define PSP_SCR_H 272
#endif // PROCYON_PSP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef PROCYON_DESKTOP
    typedef struct papp_glfw_state
    {
        GLFWwindow *window;
    } papp_glfw_state;
#endif // PROCYON_DESKTOP

typedef struct papp_state
{
    const char *title;
    bool should_close;
    int width, height;
    bool keys[PAPP_DIRECTION_COUNT];

    #ifdef PROCYON_DESKTOP
        papp_glfw_state glfw;
    #endif
} papp_state;
papp_state papp = {0};

#ifdef PROCYON_DESKTOP

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

    if(action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        switch(key)
        {
            case GLFW_KEY_W: papp.keys[PAPP_DIRECTION_UP] = (action == GLFW_PRESS); break;
            case GLFW_KEY_A: papp.keys[PAPP_DIRECTION_LEFT] = (action == GLFW_PRESS); break;
            case GLFW_KEY_S: papp.keys[PAPP_DIRECTION_DOWN] = (action == GLFW_PRESS); break;
            case GLFW_KEY_D: papp.keys[PAPP_DIRECTION_RIGHT] = (action == GLFW_PRESS); break;
            default: break;
        }
    }
}

static void papp_glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    papp.width = width;
    papp.height = height;
}

static bool papp_desktop_init(int width, int height, const char *title)
{
    glfwSetErrorCallback(papp_glfw_error_callback);

    if(!glfwInit())
        return false;

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    papp.width = width;
    papp.height = height;
    papp.title = title != NULL && title[0] != '\0' ? title : " ";
    papp.glfw.window = glfwCreateWindow(papp.width, papp.height, papp.title, NULL, NULL);

    if(!papp.glfw.window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(papp.glfw.window);
    glfwSetKeyCallback(papp.glfw.window, papp_glfw_key_callback);
    glfwSetFramebufferSizeCallback(papp.glfw.window, papp_glfw_framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    glfwSwapInterval(1);

    pgfx_init();
    pgfx_update_viewport(papp.width, papp.height);
    return true;
}

static void papp_desktop_terminate()
{
    pgfx_terminate();
    glfwDestroyWindow(papp.glfw.window);
    glfwTerminate();
}

static bool papp_desktop_should_close()
{
    papp.should_close = glfwWindowShouldClose(papp.glfw.window);
    return papp.should_close;
}

static void papp_desktop_start_frame()
{
    pgfx_update_viewport(papp.width, papp.height);
    pgfx_start_frame();
}

static void papp_desktop_end_frame()
{
    pgfx_end_frame();
    glfwSwapBuffers(papp.glfw.window);
    glfwPollEvents();
}

#endif // PROCYON_DESKTOP

#ifdef PROCYON_PSP

static int papp_psp_exit_callback()
{
    papp.should_close = 1;
    return 0;
}

static int papp_psp_callback_thread(SceSize args, void *argp)
{
	int cbid = sceKernelCreateCallback("Exit Callback", papp_psp_exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
}

static int papp_psp_setup_callbacks()
{
    int thid = 0;
	thid = sceKernelCreateThread("update_thread", papp_psp_callback_thread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
		sceKernelStartThread(thid, 0, 0);
	return thid;
}

static bool papp_psp_init()
{
    if(papp_psp_setup_callbacks() < 0)
        return false;

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    pgfx_init();
}

static void papp_psp_terminate()
{
    pgfx_terminate();
    sceKernelExitGame();
}

static bool papp_psp_should_close()
{
    return papp.should_close;
}

static void papp_psp_start_frame()
{
    SceCtrlData ctrl_data;
    sceCtrlReadBufferPositive(&ctrl_data, 1);
    papp.keys[PAPP_DIRECTION_UP] = ctrl_data.Buttons & PSP_CTRL_UP;
    papp.keys[PAPP_DIRECTION_LEFT] = ctrl_data.Buttons & PSP_CTRL_LEFT;
    papp.keys[PAPP_DIRECTION_DOWN] = ctrl_data.Buttons & PSP_CTRL_DOWN;
    papp.keys[PAPP_DIRECTION_RIGHT] = ctrl_data.Buttons & PSP_CTRL_RIGHT;

    pgfx_start_frame();
}

static void papp_psp_end_frame()
{
    pgfx_end_frame();
}

#endif // PROCYON_PSP

void papp_init(int width, int height, const char *title)
{
    #if defined(PROCYON_DESKTOP)
        papp_desktop_init(width, height, title);
    #elif defined(PROCYON_PSP)
        papp_psp_init();
    #endif
}

void papp_terminate()
{
    #if defined(PROCYON_DESKTOP)
        papp_desktop_terminate();
    #elif defined(PROCYON_PSP)
        papp_psp_terminate();
    #endif
}

bool papp_should_close()
{
    #if defined(PROCYON_DESKTOP)
        return papp_desktop_should_close();
    #elif defined(PROCYON_PSP)
        return papp_psp_should_close();
    #endif
}

void papp_start_frame()
{
    #if defined(PROCYON_DESKTOP)
        papp_desktop_start_frame();
    #elif defined(PROCYON_PSP)
        papp_psp_start_frame();
    #endif
}

void papp_end_frame()
{
    #if defined(PROCYON_DESKTOP)
        papp_desktop_end_frame();
    #elif defined(PROCYON_PSP)
        papp_psp_end_frame();
    #endif
}

void papp_set_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    pgfx_set_clear_color(r, g, b, a);
}

void papp_clear()
{
    pgfx_clear();
}

bool papp_key_down(papp_direction direction)
{
    return papp.keys[direction];
}