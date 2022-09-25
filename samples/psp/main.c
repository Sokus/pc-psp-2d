#include "procyon.h"

int main()
{
    papp_init(960, 480, "Procyon App");
    papp_set_clear_color(46, 34, 47, 255);

    papp_texture texture = papp_load_texture("red.png");

    float player_x = 100.0f;
    float player_y = 100.0f;

    while (!papp_should_close())
    {
        papp_start_frame();
        papp_clear();

        float dt = 0.0166f;
        if(papp_key_down(PAPP_DIRECTION_LEFT)) player_x -= (200 * dt);
        if(papp_key_down(PAPP_DIRECTION_RIGHT)) player_x += (200 * dt);
        if(papp_key_down(PAPP_DIRECTION_UP)) player_y -= (200 * dt);
        if(papp_key_down(PAPP_DIRECTION_DOWN)) player_y += (200 * dt);

        papp_draw_texture(texture, player_x, player_y, 4.0f);

        papp_end_frame();
    }

    papp_terminate();
    return 0;
}


























