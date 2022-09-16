#ifndef PROCYON_H
#define PROCYON_H


#if defined(_WIN32)
    #define PAPP_DESKTOP
#endif

#if !defined(PROCYON_DEBUG) && !defined(NDEBUG)
    #define PROCYON_DEBUG
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void papp_main();

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // PROCYON_H