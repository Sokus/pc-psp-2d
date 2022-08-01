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

#include "pc_log.h"
