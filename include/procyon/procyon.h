#ifndef PROCYON_H
#define PROCYON_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef PC_MALLOC
#define PC_MALLOC(size) malloc(size)
#endif

#ifndef PC_FREE
#define PC_FREE(ptr) free(ptr)
#endif

#ifndef PC_REALLOC
#define PC_REALLOC(ptr, size) realloc(ptr, size)
#endif

#include <stdint.h>

typedef enum LogLevel
{
    LOG_ALL = 0,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL,
    LOG_NONE,
} LogLevel;

#ifdef __cplusplus
extern "C" {
#endif

void pcInitWindow(int width, int height, const char *title);
void pcCloseWindow();
void pcBeginFrame();
void pcEndFrame();
bool pcWindowShouldClose();

void pcSetLogLevel(int log_level);
void pcLog(int log_level, const char *text, ...);

unsigned char *pcLoadFileData(const char *file_name, unsigned int *bytes_read);
void pcUnloadFileData(unsigned char *data);
char *pcLoadFileText(const char *file_name);
void pcUnloadFileText(char *text);

void pcConcatenateStrings(const char *str_a, size_t str_a_size,
                          const char *str_b, size_t str_b_size,
                          char *dest, size_t dest_size);
unsigned int pcStringLength(const char *str);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // PROCYON_H