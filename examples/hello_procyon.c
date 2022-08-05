#include "procyon/procyon.h"

int main()
{
    pcInitWindow(640, 480, "Hello, Procyon!");

    while(!pcWindowShouldClose())
    {
        pcBeginFrame();

        pcEndFrame();
    }

    pcCloseWindow();

    return 0;
}