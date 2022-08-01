#ifndef PROCYON_LOG_H
#define PROCYON_LOG_H

#include "config.h"

#if defined(ENABLE_LOG)
    #define PC_LOG(log_level, ...) pcLog(log_level, __VA_ARGS__)
    #if defined(ENABLE_DEBUG)
        #define PC_LOG_DEBUG(...) pcLog(LOG_DEBUG, __VA_ARGS__)
    #else
        #define PC_LOG_DEBUG(...) (void)0
    #endif
#else
    #define PC_LOG(log_level, ...) (void)0
    #define PC_LOG_DEBUG(...) (void)0
#endif

#endif // PROCYON_LOG_H