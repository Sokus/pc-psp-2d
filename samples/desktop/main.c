#include "procyon.h"

int main()
{
    papp_init(960, 480, "Nie patrz mi się na tytuł >:(");
    papp_set_clear_color(46, 34, 47, 255);

    papp_texture texture = papp_load_texture("red.png");

    while (!papp_should_close())
    {
        papp_start_frame();
        papp_clear();

        papp_draw_texture(texture, 16.0f, 16.0f, 16.0f);
        papp_draw_texture(texture, 200.0f, 300.0f, 16.0f);
        papp_draw_texture(texture, 500.0f, 120.0f, 16.0f);

        papp_end_frame();
    }

    papp_terminate();
    return 0;
}