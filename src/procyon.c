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

        unsigned int width;
        unsigned int height;
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

void pcInitWindow(int width, int height, const char *title)
{

}