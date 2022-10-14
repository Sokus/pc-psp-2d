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
#include <stdbool.h>

#ifdef PROCYON_DESKTOP
    typedef struct papp_glfw_state
    {
        GLFWwindow *window;
    } papp_glfw_state;

    typedef struct papp_keyboard
    {
        bool key_state[PAPP_KEY_COUNT];
        bool key_state_previous[PAPP_KEY_COUNT];
    } papp_keyboard;
#endif // PROCYON_DESKTOP

typedef struct papp_gamepad
{
    bool button_state[PAPP_BUTTON_COUNT];
    bool button_state_previous[PAPP_BUTTON_COUNT];
    float axis_state[PAPP_AXIS_COUNT];
} papp_gamepad;

typedef struct papp_input
{
    papp_gamepad gamepad;

    #ifdef PROCYON_DESKTOP
        papp_keyboard keyboard;
    #endif
} papp_input;

typedef struct papp_state
{
    const char *title;
    bool should_close;
    int width, height;

    papp_input input;

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
        bool is_down = (action == GLFW_PRESS);
        papp.input.keyboard.key_state[key] = is_down;
    }
}

static void papp_glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    papp.width = width;
    papp.height = height;
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

static float papp_psp_process_axis_value(unsigned char value)
{
    // Axis have values between 0 and 255, 128 is the middle value.
    // This means the left range is 128 values
    // while the right range is 126 values.
    float result = 0.0f;
    if(value < 128)
        result = ((float)value - 128.0f) / 128.0f;
    else if(value > 128)
        result = ((float)value - 128.0f) / 126.0f;
    return result;
}

#endif // PROCYON_PSP

void papp_init(int width, int height, const char *title)
{
    papp.title = title != 0 && title[0] != 0 ? title : " ";

    #if defined(PROCYON_DESKTOP)
        glfwSetErrorCallback(papp_glfw_error_callback);

        if(!glfwInit())
            return;

        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        papp.width = width;
        papp.height = height;

        papp.glfw.window = glfwCreateWindow(papp.width, papp.height, papp.title, NULL, NULL);

        if(!papp.glfw.window)
        {
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(papp.glfw.window);
        glfwSetKeyCallback(papp.glfw.window, papp_glfw_key_callback);
        glfwSetFramebufferSizeCallback(papp.glfw.window, papp_glfw_framebuffer_size_callback);

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            glfwTerminate();
            return;
        }

        glfwSwapInterval(1);
    #elif defined(PROCYON_PSP)
        if(papp_psp_setup_callbacks() < 0)
            return;

        papp.width = PSP_SCR_W;
        papp.height = PSP_SCR_H;

        sceCtrlSetSamplingCycle(0);
        sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    #endif

    pgfx_init();
    pgfx_update_viewport(papp.width, papp.height);
    pgfx_ortho(0.0f, (float)papp.width, (float)papp.width, 0.0f, -1.0f, 1.0f);
}

void papp_terminate()
{
    pgfx_terminate();

    #if defined(PROCYON_DESKTOP)
        glfwDestroyWindow(papp.glfw.window);
        glfwTerminate();
    #elif defined(PROCYON_PSP)
        sceKernelExitGame();
    #endif
}

bool papp_should_close()
{
    #if defined(PROCYON_DESKTOP)
        papp.should_close = glfwWindowShouldClose(papp.glfw.window);
    #endif

    return papp.should_close;
}

void papp_start_frame()
{
    #if defined(PROCYON_DESKTOP)
        for(int jid = GLFW_JOYSTICK_1; jid < GLFW_JOYSTICK_LAST; jid++)
        {
            GLFWgamepadstate state;
            if(!glfwJoystickIsGamepad(jid)) continue;
            if(!glfwGetGamepadState(jid, &state)) continue;

            for(int button = 0; button < PAPP_BUTTON_COUNT; button++)
                papp.input.gamepad.button_state[button] = state.buttons[button];

            for(int axis = 0; axis < PAPP_AXIS_COUNT; axis++)
                papp.input.gamepad.axis_state[axis] = state.axes[axis];
        }
    #elif defined(PROCYON_PSP)
        SceCtrlData ctrl_data;
        sceCtrlReadBufferPositive(&ctrl_data, 1);

        papp.input.gamepad.button_state[PAPP_BUTTON_CROSS] = ctrl_data.Buttons & PSP_CTRL_CROSS;
        papp.input.gamepad.button_state[PAPP_BUTTON_CIRCLE] = ctrl_data.Buttons & PSP_CTRL_CIRCLE;
        papp.input.gamepad.button_state[PAPP_BUTTON_SQUARE] = ctrl_data.Buttons & PSP_CTRL_SQUARE;
        papp.input.gamepad.button_state[PAPP_BUTTON_TRIANGLE] = ctrl_data.Buttons & PSP_CTRL_TRIANGLE;
        papp.input.gamepad.button_state[PAPP_BUTTON_LEFT_BUMPER] = ctrl_data.Buttons & PSP_CTRL_LTRIGGER;
        papp.input.gamepad.button_state[PAPP_BUTTON_RIGHT_BUMPER] = ctrl_data.Buttons & PSP_CTRL_RTRIGGER;
        papp.input.gamepad.button_state[PAPP_BUTTON_BACK] = ctrl_data.Buttons & PSP_CTRL_SELECT;
        papp.input.gamepad.button_state[PAPP_BUTTON_START] = ctrl_data.Buttons & PSP_CTRL_START;
        papp.input.gamepad.button_state[PAPP_BUTTON_DPAD_DOWN] = ctrl_data.Buttons & PSP_CTRL_DOWN;
        papp.input.gamepad.button_state[PAPP_BUTTON_DPAD_LEFT] = ctrl_data.Buttons & PSP_CTRL_LEFT;
        papp.input.gamepad.button_state[PAPP_BUTTON_DPAD_RIGHT] = ctrl_data.Buttons & PSP_CTRL_RIGHT;
        papp.input.gamepad.button_state[PAPP_BUTTON_DPAD_UP] = ctrl_data.Buttons & PSP_CTRL_UP;

        papp.input.gamepad.axis_state[PAPP_AXIS_LEFT_X] = papp_psp_process_axis_value(ctrl_data.Lx);
        papp.input.gamepad.axis_state[PAPP_AXIS_LEFT_Y] = papp_psp_process_axis_value(ctrl_data.Ly);
    #endif

    pgfx_update_viewport(papp.width, papp.height);
    pgfx_ortho(0.0f, (float)papp.width, (float)papp.width, 0.0f, -1.0f, 1.0f);
    pgfx_start_frame();
}

void papp_end_frame()
{
    pgfx_end_frame();

    memcpy(papp.input.gamepad.button_state_previous, papp.input.gamepad.button_state, sizeof(papp.input.gamepad.button_state));

    #if defined(PROCYON_DESKTOP)
        memcpy(papp.input.keyboard.key_state_previous, papp.input.keyboard.key_state, sizeof(papp.input.keyboard.key_state));

        glfwSwapBuffers(papp.glfw.window);
        glfwPollEvents();
    #endif
}

void papp_clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    pgfx_set_clear_color(r, g, b, a);
    pgfx_clear();
}

void papp_enable_render_target(papp_render_target *render_target)
{
    pgfx_render_batch();
    pgfx_bind_render_target(render_target);
    int width = render_target->texture.width;
    int height = render_target->texture.height;
    pgfx_update_viewport(width, height);

    #if defined(PROCYON_DESKTOP)
        pgfx_ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
        pgfx_front_face(PGFX_CW);
    #elif defined(PROCYON_PSP)
        pgfx_ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    #endif

}

void papp_disable_render_target(void *temp_fb)
{
    pgfx_render_batch();
    pgfx_unbind_render_target();
    pgfx_update_viewport(papp.width, papp.height);
    pgfx_ortho(0.0f, (float)papp.width, (float)papp.height, 0.0f, -1.0f, 1.0f);

    #if defined(PROCYON_DESKTOP)
        pgfx_front_face(PGFX_CCW);
    #endif
}

bool papp_key_down(papp_key key)
{
    bool is_down = false;
    #if defined(PROCYON_DESKTOP)
        if(key >= 0 && key < PAPP_KEY_COUNT)
            is_down = papp.input.keyboard.key_state[key];
    #endif
    return is_down;
}

bool papp_key_pressed(papp_key key)
{
    bool pressed = false;
    #if defined(PROCYON_DESKTOP)
        if(key >= 0 && key < PAPP_KEY_COUNT)
            pressed = (papp.input.keyboard.key_state[key] &&
                       !papp.input.keyboard.key_state_previous[key]);
    #endif
    return pressed;
}

bool papp_key_released(papp_key key)
{
    bool released = false;
    #if defined(PROCYON_DESKTOP)
        if(key >= 0 && key < PAPP_KEY_COUNT)
            released = (!papp.input.keyboard.key_state[key] &&
                        papp.input.keyboard.key_state_previous[key]);
    #endif
    return released;
}

bool papp_button_down(papp_gamepad_button button)
{
    bool is_down = false;
    if(button >= 0 && button < PAPP_BUTTON_COUNT)
        is_down = papp.input.gamepad.button_state[button];
    return is_down;
}

bool papp_button_pressed(papp_gamepad_button button)
{
    bool pressed = false;
    if(button >= 0 && button < PAPP_BUTTON_COUNT)
        pressed = (papp.input.gamepad.button_state[button] && !papp.input.gamepad.button_state_previous[button]);
    return pressed;
}

bool papp_button_released(papp_gamepad_button button)
{
    bool released = false;
    if(button >= 0 && button < PAPP_BUTTON_COUNT)
        released = (!papp.input.gamepad.button_state[button] && papp.input.gamepad.button_state_previous[button]);
    return released;
}

float papp_axis(papp_gamepad_axis axis)
{
    float value = 0.0f;
    if(axis >= 0 && axis < PAPP_AXIS_COUNT)
        value = papp.input.gamepad.axis_state[axis];
    return value;
}
