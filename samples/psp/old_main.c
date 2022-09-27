// Include Graphics Libraries
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>

#include "stb_image.h"

#include <string.h>
#include <malloc.h>

#define PSP_BUF_WIDTH 512
#define PSP_SCR_W 480
#define PSP_SCR_H 272

PSP_MODULE_INFO("Procyon App", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

int running = 1;

typedef struct pgfx_vec2
{
    float u, v;
} pgfx_vec2;

typedef struct pgfx_color
{
    float r, g, b, a;
} pgfx_color;

typedef struct pgfx_vec3
{
    float x, y, z;
} pgfx_vec3;

typedef struct pgfx_psp_vertex
{
    pgfx_vec2 texcoord;
    unsigned int color;
    pgfx_vec3 pos;
} pgfx_psp_vertex;

typedef struct pgfx_state
{
    unsigned int __attribute__((aligned(16))) list[262144];
    pgfx_psp_vertex __attribute__((aligned(16))) vertex_data[4];
    int vertex_data_used;
    int vertex_data_start;
} pgfx_state;
pgfx_state pgfx = {0};


#if 1

int papp_exit_callback(int arg1, int arg2, void *common)
{
	running = 0;
	return 0;
}

int papp_callback_thread(SceSize arg_count, void *args)
{
	int cbid = sceKernelCreateCallback("Exit Callback", papp_exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

int papp_setup_callbacks()
{
	int thread_id = sceKernelCreateThread("update_thread", papp_callback_thread, 0x11, 0xFA0, 0, 0);
	if (thread_id >= 0)
		sceKernelStartThread(thread_id, 0, 0);
	return thread_id;
}

static unsigned int pgfx_get_buffer_size(unsigned int width, unsigned int height, unsigned int pixel_format)
{
	switch (pixel_format)
	{
		case GU_PSM_T4:
			return (width * height) >> 1;

		case GU_PSM_T8:
			return width * height;

		case GU_PSM_5650:
		case GU_PSM_5551:
		case GU_PSM_4444:
		case GU_PSM_T16:
			return 2 * width * height;

		case GU_PSM_8888:
		case GU_PSM_T32:
			return 4 * width * height;

		default:
			return 0;
	}
}

void* pgfx_get_static_vram_buffer(unsigned int width, unsigned int height, unsigned int pixel_format)
{
	static unsigned int static_offset = 0;
	unsigned int size = pgfx_get_buffer_size(width, height, pixel_format);
	void* result = (void*)static_offset;
	static_offset += size;
	return result;
}

void* pgfx_get_static_vram_texture(unsigned int width, unsigned int height, unsigned int pixel_format)
{
	void* result = pgfx_get_static_vram_buffer(width, height, pixel_format);
	return (void*)(((unsigned int)result) + ((unsigned int)sceGeEdramGetAddr()));
}

#endif

void pgfx_init() {
    void* fbp0 = pgfx_get_static_vram_buffer(PSP_BUF_WIDTH, PSP_SCR_H, GU_PSM_8888);
	void* fbp1 = pgfx_get_static_vram_buffer(PSP_BUF_WIDTH, PSP_SCR_H, GU_PSM_8888);
	void* zbp = pgfx_get_static_vram_buffer(PSP_BUF_WIDTH, PSP_SCR_H, GU_PSM_4444);

	sceGuInit();

	sceGuStart(GU_DIRECT, pgfx.list);
	sceGuDrawBuffer(GU_PSM_8888, fbp0, PSP_BUF_WIDTH);
	sceGuDispBuffer(PSP_SCR_W, PSP_SCR_H, fbp1, PSP_BUF_WIDTH);
	sceGuDepthBuffer(zbp, PSP_BUF_WIDTH);
	sceGuOffset(2048 - (PSP_SCR_W/2), 2048 - (PSP_SCR_H/2));
	sceGuViewport(2048, 2048, PSP_SCR_W, PSP_SCR_H);
	sceGuDepthRange(65535, 0);
	sceGuScissor(0, 0, PSP_SCR_W, PSP_SCR_H);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CCW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumOrtho(0, PSP_SCR_W, PSP_SCR_H, 0, -10.0f, 10.0f);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();

    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
}

void pgfx_start_frame()
{
    sceGuStart(GU_DIRECT, pgfx.list);
}

void pgfx_end_frame()
{
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

void pgfx_terminate()
{
    sceGuTerm();
}

void pgfx_swizzle_fast(u8 *out, u8 *in, unsigned int width, unsigned int height)
{
    unsigned int width_blocks = (width / 16);
    unsigned int height_blocks = (height / 8);

    unsigned int src_pitch = (width - 16) / 4;
    unsigned int src_row = width * 8;

    u8 *ysrc = in;
    u32 *dst = (u32 *)out;

    for (unsigned int blocky = 0; blocky < height_blocks; ++blocky)
    {
        u8 *xsrc = ysrc;
        for (unsigned int blockx = 0; blockx < width_blocks; ++blockx)
        {
            u32 *src = (u32 *)xsrc;
            for (unsigned int j = 0; j < 8; ++j)
            {
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                src += src_pitch;
            }
            xsrc += 16;
        }
        ysrc += src_row;
    }
}

unsigned int papp_closest_greater_pow2(unsigned int value)
{
    unsigned int poweroftwo = 1;
    while (poweroftwo < value)
        poweroftwo <<= 1;
    return poweroftwo;
}

void papp_copy_texture_data(void* dest, void* src, int padded_width, int width, int height)
{
    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            ((unsigned int*)dest)[x + y * padded_width] = ((unsigned int *)src)[x + y * width];
        }
    }
}

typedef struct papp_texture
{
    unsigned int width, height;
    unsigned int padded_width, padded_height;
    void* data;
} papp_texture;

papp_texture* papp_load_texture(char* filename, int vram)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(GU_TRUE);
    unsigned char *data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if(!data)
        return NULL;

    papp_texture* tex = (papp_texture*)malloc(sizeof(papp_texture));
    tex->width = width;
    tex->height = height;
    tex->padded_width = papp_closest_greater_pow2(width);
    tex->padded_height = papp_closest_greater_pow2(height);

    unsigned int *data_buffer = (unsigned int *)memalign(16, tex->padded_height * tex->padded_width * 4);

    papp_copy_texture_data(data_buffer, data, tex->padded_width, tex->width, tex->height);

    stbi_image_free(data);

    unsigned int* swizzled_pixels = NULL;
    size_t size = tex->padded_height * tex->padded_width * 4;
    if(vram)
        swizzled_pixels = pgfx_get_static_vram_texture(tex->padded_width, tex->padded_height, GU_PSM_8888);
    else
        swizzled_pixels = (unsigned int *)memalign(16, size);

    pgfx_swizzle_fast((u8*)swizzled_pixels, (u8*)data_buffer, tex->padded_width * 4, tex->padded_height);

    free(data_buffer);
    tex->data = swizzled_pixels;

    sceKernelDcacheWritebackInvalidateAll();

    return tex;
}

void pgfx_bind_texture(papp_texture* tex)
{
    if(tex == NULL)
        return;

    sceGuTexMode(GU_PSM_8888, 0, 0, 1);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexWrap(GU_REPEAT, GU_REPEAT);
    sceGuTexImage(0, tex->padded_width, tex->padded_height, tex->padded_width, tex->data);
}

#define PGFX_COLOR(r, g, b, a) ((u32)((u8)r) | (u32)((u8)g) << 8 | (u32)((u8)b) << 16 | (u32)((u8)a) << 24)

int main()
{
    papp_setup_callbacks();

    pgfx_init();

    papp_texture* texture = papp_load_texture("red.png", GU_TRUE);

    while(running)
    {
        pgfx_start_frame();

        sceGuDisable(GU_DEPTH_TEST);

        sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
        sceGuEnable(GU_BLEND);

        unsigned int color = PGFX_COLOR(46, 34, 47, 255);
        sceGuClearColor(color);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_STENCIL_BUFFER_BIT);

        pgfx_bind_texture(texture);

        unsigned int vcolor = 0xFFFFFFFF;

        pgfx.vertex_data_used = 0;
        {
            pgfx_psp_vertex *vertex;
            pgfx.vertex_data_start = pgfx.vertex_data_used;
            vertex = pgfx.vertex_data + pgfx.vertex_data_used;
            vertex->texcoord.u = 0.0f; vertex->texcoord.v = 1.0f;
            vertex->color = vcolor;
            vertex->pos.x = 0.0f; vertex->pos.y = 0.0f, vertex->pos.z = -1.0f;
            pgfx.vertex_data_used++;

            vertex = pgfx.vertex_data + pgfx.vertex_data_used;
            vertex->texcoord.u = 1.0f; vertex->texcoord.v = 0.0f;
            vertex->color = vcolor;
            vertex->pos.x = 16.0f; vertex->pos.y = 16.0f, vertex->pos.z = -1.0f;
            pgfx.vertex_data_used++;

            sceGumDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 2, 0, pgfx.vertex_data + pgfx.vertex_data_start);
        }

        sceGuFinish();
        sceGuSync(0, 0);
        pgfx.vertex_data_used = 0;
        sceGuStart(GU_DIRECT, pgfx.list);

        {
            pgfx_psp_vertex *vertex;
            pgfx.vertex_data_start = pgfx.vertex_data_used;
            vertex = pgfx.vertex_data + pgfx.vertex_data_used;
            vertex->texcoord.u = 0.0f; vertex->texcoord.v = 1.0f;
            vertex->color = vcolor;
            vertex->pos.x = 32.0f; vertex->pos.y = 32.0f, vertex->pos.z = -1.0f;
            pgfx.vertex_data_used++;

            vertex = pgfx.vertex_data + pgfx.vertex_data_used;
            vertex->texcoord.u = 1.0f; vertex->texcoord.v = 0.0f;
            vertex->color = vcolor;
            vertex->pos.x = 48.0f; vertex->pos.y = 48.0f, vertex->pos.z = -1.0f;
            pgfx.vertex_data_used++;

            sceGumDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 2, 0, pgfx.vertex_data + pgfx.vertex_data_start);
        }

        pgfx_end_frame();
    }

cleanup:
    pgfx_terminate();

    sceKernelExitGame();
    return 0;
}