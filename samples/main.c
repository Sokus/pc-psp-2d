#include "procyon.h"

int main(int argc, char* argv[])
{
    papp_init(960, 540, "Test");

    papp_texture red = papp_load_texture("red.png");
    papp_render_target render_target = papp_create_render_target(64, 64);

	while(!papp_should_close())
	{
        papp_start_frame();

        papp_enable_render_target(&render_target);
        {
            papp_clear(0, 12, 24, 255);

            papp_rect source = {0.0f, 0.0f, 8.0f, 8.0f};
            papp_rect destination = {0.0f, 0.0f, 32.0f, 32.0f};
            papp_draw_texture_rect(red, source, destination);
        }
        papp_disable_render_target();

        papp_clear(20, 0, 12, 255);

        papp_rect source = {0.0f, 0.0f, 64.0f, 64.0f};
        papp_rect destination = {0.0f, 0.0f, 256.0f, 256.0f};
        papp_draw_texture_rect(render_target.texture, source, destination);

        papp_end_frame();
	}

    papp_terminate();

	return 0;
}

