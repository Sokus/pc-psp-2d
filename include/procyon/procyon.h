#ifndef PROCYON_H
#define PROCYON_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifndef NULL
#define NULL ((void *)0)
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

void pcSetLogLevel(int log_level);
void pcLog(int log_level, const char *text, ...);

void pcConcatenateStrings(const char *str_a, size_t str_a_size,
                          const char *str_b, size_t str_b_size,
                          char *dest, size_t dest_size);
unsigned int pcStringLength(const char *str);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // PROCYON_H