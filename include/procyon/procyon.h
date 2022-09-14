#ifndef PROCYON_H
#define PROCYON_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PLATFORM_DESKTOP

void pcInitWindow(int width, int height, const char *title);
void pcCloseWindow();
void pcBeginFrame();
void pcEndFrame();
bool pcWindowShouldClose();

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // PROCYON_H