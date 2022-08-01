#include "procyon.h"
#include "config.h"

#include <stdlib.h> // exit()
#include <stdio.h> // fprintf(), vprintf()
#include <stdarg.h> // va_list, va_start(), va_end()
#include <string.h> // strcpy(), strcat()

#ifndef MAX_LOG_MESSAGE_LENGTH
#define MAX_LOG_MESSAGE_LENGTH 128
#endif

static int g_log_level = LOG_INFO;

void pcSetLogLevel(int log_level)
{
    g_log_level = log_level;
}

void pcLog(int log_level, const char *text, ...)
{
#if defined(ENABLE_LOG)
    if (log_level < g_log_level) return;

    va_list args;
    va_start(args, text);

    char buffer[MAX_LOG_MESSAGE_LENGTH] = {0};

    switch (log_level)
    {
        case LOG_DEBUG: strcpy(buffer, "DEBUG: "); break;
        case LOG_INFO: strcpy(buffer, "INFO: "); break;
        case LOG_WARNING: strcpy(buffer, "WARNING: "); break;
        case LOG_ERROR: strcpy(buffer, "ERROR: "); break;
        case LOG_FATAL: strcpy(buffer, "FATAL: "); break;
        default: break;
    }

    strcat(buffer, text);
    strcat(buffer, "\n");
    vprintf(buffer, args);
    fflush(stdout);

    va_end(args);

    if (log_level == LOG_FATAL) exit(EXIT_FAILURE);
#endif // ENABLE_LOG
}


