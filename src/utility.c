#include "procyon.h"
#include "config.h"
#include "utility.h"

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

unsigned char *pcLoadFileData(const char *file_name, unsigned int *bytes_read)
{
    unsigned char *data = NULL;
    *bytes_read = 0;

    if (file_name != NULL)
    {
        FILE *file = fopen(file_name, "rb");

        if (file != NULL)
        {
            // WARNING: On binary streams SEEK_END could not be found,
            // using fseek() and ftell() could not work in some (rare) cases
            fseek(file, 0, SEEK_END);
            unsigned int size = (unsigned int)ftell(file);
            fseek(file, 0, SEEK_SET);

            if (size > 0)
            {
                data = (unsigned char *)PC_MALLOC(size*sizeof(unsigned char));

                // NOTE: fread() returns number of read elements instead of bytes, so we read [1 byte, size elements]
                unsigned int count = (unsigned int)fread(data, sizeof(unsigned char), size, file);
                *bytes_read = count;

                if(count != size)
                {
                    PC_LOG(LOG_WARNING, "FILEIO: [%s] File partially loaded", file_name);
                }
                else
                {
                    PC_LOG(LOG_INFO, "FILEIO: [%s] File loaded successfully", file_name);
                }
            }
            else
            {
                PC_LOG(LOG_WARNING, "FILEIO: [%s] Failed to read file", file_name);
            }

            fclose(file);
        }
        else
        {
            PC_LOG(LOG_WARNING, "FILEIO: [%s] Failed to open file", file_name);
        }
    }
    else
    {
        PC_LOG(LOG_WARNING, "FILEIO: File name provided is not valid");
    }

    return data;
}

void pcUnloadFileData(unsigned char *data)
{
    PC_FREE(data);
}

char *pcLoadFileText(const char *file_name)
{
    char *text = NULL;

    if(file_name != NULL)
    {
        FILE *file = fopen(file_name, "rt");

        if(file != NULL)
        {
            fseek(file, 0, SEEK_END);
            unsigned int size = (unsigned int)ftell(file);
            fseek(file, 0, SEEK_SET);

            if(size > 0)
            {
                text = (char *)PC_MALLOC((size + 1) * sizeof(char));
                unsigned int count = (unsigned int)fread(text, sizeof(char), size, file);

                // WARNING: \r\n is converted to \n on reading, so,
                // read bytes count gets reduced by the number of lines
                if (count < size) text = PC_REALLOC(text, count + 1);

                text[count] = '\0';

                PC_LOG(LOG_INFO, "FILEIO: [%s] Text file loaded successfully", file_name);
            }
            else
            {
                PC_LOG(LOG_WARNING, "FILEIO: [%s] Failed to read text file", file_name);
            }

            fclose(file);
        }
        else
        {
            PC_LOG(LOG_WARNING, "FILEIO: [%s] Failed to open text file", file_name);
        }
    }
    else
    {
        PC_LOG(LOG_WARNING, "FILEIO: File name provided is not valid");
    }

    return text;
}

void pcUnloadFileText(char *text)
{
    PC_FREE(text);
}
