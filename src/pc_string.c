#include "procyon.h"

#include <string.h> // memcpy()

#define PCS_MAX(a, b) (((a)>(b)) ? (a) : (b))
#define PCS_MIN(a, b) (((a)<(b)) ? (a) : (b))

void pcConcatenateStrings(const char *str_a, size_t str_a_size,
                          const char *str_b, size_t str_b_size,
                          char *dest, size_t dest_size)
{
    size_t dest_size_clamped = PCS_MAX(0, dest_size);
    size_t str_a_size_clamped = PCS_MIN(str_a_size, dest_size_clamped);
    size_t size_remaining = dest_size_clamped - str_a_size_clamped;
    size_t str_b_size_clamped = PCS_MIN(str_b_size, size_remaining);

    memcpy(dest, str_a, str_a_size_clamped);
    memcpy(dest+str_a_size_clamped, str_b, str_b_size_clamped);

    *(dest+str_a_size_clamped+str_b_size_clamped) = 0;
}

unsigned int pcStringLength(const char *str)
{
    unsigned int result = 0;
    while(*str++)
    {
        ++result;
    }
    return result;
}
