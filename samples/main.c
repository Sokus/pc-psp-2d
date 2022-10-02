#include "procyon.h"

int main()
{
    papp_init(960, 480, "Procyon App");
    papp_set_clear_color(46, 34, 47, 255);

    papp_texture texture = papp_load_texture("atlas.png");

    float player_x = 100.0f;
    float player_y = 100.0f;

    float rotation = 0;

    while (!papp_should_close())
    {
        papp_start_frame();
        papp_clear();

        float dt = 0.0166f;

        float input_x = (-1.0f * (float)papp_key_is_down(PAPP_KEY_A) + (float)papp_key_is_down(PAPP_KEY_D) +
                         -1.0f * (float)papp_gamepad_button_is_down(PAPP_GAMEPAD_BUTTON_DPAD_LEFT) +
                         (float)papp_gamepad_button_is_down(PAPP_GAMEPAD_BUTTON_DPAD_RIGHT) +
                         papp_axis_value(PAPP_GAMEPAD_AXIS_LEFT_X));
        float input_y = (-1.0f * (float)papp_key_is_down(PAPP_KEY_W) + (float)papp_key_is_down(PAPP_KEY_S) +
                         -1.0f * (float)papp_gamepad_button_is_down(PAPP_GAMEPAD_BUTTON_DPAD_UP) +
                         (float)papp_gamepad_button_is_down(PAPP_GAMEPAD_BUTTON_DPAD_DOWN) +
                         papp_axis_value(PAPP_GAMEPAD_AXIS_LEFT_Y));
        input_x = input_x > 1.0f ? 1.0f : input_x < -1.0f ? -1.0f : input_x;
        input_y = input_y > 1.0f ? 1.0f : input_y < -1.0f ? -1.0f : input_y;

        player_x += 200.0f * input_x * dt;
        player_y += 200.0f * input_y * dt;

        papp_vec2 tile = { 0.0f, 0.0f };
        papp_rect source = { tile.x * 8.0f, tile.y * 8.0f, 8.0f, 8.0f };
        papp_rect dest = { player_x, player_y, 32.0f, 32.0f };
        papp_draw_texture_rect(texture, source, dest);

        papp_end_frame();
    }

    papp_terminate();
    return 0;
}


























