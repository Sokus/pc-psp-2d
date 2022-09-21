#include "procyon.h"

int main()
{
    papp_init(960, 480, "Nie patrz mi sie na tytul");

    papp_texture texture = papp_load_texture("red.png");

    while (!papp_should_close())
    {
        papp_clear(46, 34, 47, 255);

        papp_draw_texture(texture, 16.0f, 16.0f, 1.0f);

        papp_flip();
    }

    papp_terminate();
    return 0;
}