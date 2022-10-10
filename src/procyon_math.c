#include "procyon.h"

unsigned int papp_closest_greater_pow2(const unsigned int value)
{
    if(value == 0 || value > (1 << 31))
        return 0;
    unsigned int poweroftwo = 1;
    while (poweroftwo < value)
        poweroftwo <<= 1;
    return poweroftwo;
}

papp_mat4 papp_ortho(float left, float right, float bottom, float top, float near, float far)
{
    papp_mat4 result = {0.0f};
    result.elements[0][0] = 2.0f / (right - left);
    result.elements[1][1] = 2.0f / (top - bottom);
    result.elements[2][2] = 2.0f / (near - far);
    result.elements[3][3] = 1.0f;

    result.elements[3][0] = (left + right) / (left - right);
    result.elements[3][1] = (bottom + top) / (bottom - top);
    result.elements[3][2] = (far + near) / (near - far);

    return result;
}