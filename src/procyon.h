#ifndef PROCYON_H
#define PROCYON_H

#if defined(_WIN32)
    #define PROCYON_DESKTOP
#endif

#if 0 && !defined(PSP)
#define PSP
#endif

#if defined(PSP)
    #define PROCYON_PSP
#endif

#if !defined(PROCYON_DEBUG) && !defined(NDEBUG)
    #define PROCYON_DEBUG
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef struct papp_rect
{
    float x, y;
    float width, height;
} papp_rect;

typedef struct papp_vec2
{
    float x, y;
} papp_vec2;

typedef struct papp_vec3
{
    float x, y, z;
} papp_vec3;

typedef struct papp_color
{
    unsigned char r, g, b, a;
} papp_color;

typedef enum papp_key
{
    PAPP_KEY_SPACE         = 32,
    PAPP_KEY_APOSTROPHE    = 39,
    PAPP_KEY_COMMA         = 44,
    PAPP_KEY_MINUS         = 45,
    PAPP_KEY_PERIOD        = 46,
    PAPP_KEY_SLASH         = 47,
    PAPP_KEY_0             = 48,
    PAPP_KEY_1             = 49,
    PAPP_KEY_2             = 50,
    PAPP_KEY_3             = 51,
    PAPP_KEY_4             = 52,
    PAPP_KEY_5             = 53,
    PAPP_KEY_6             = 54,
    PAPP_KEY_7             = 55,
    PAPP_KEY_8             = 56,
    PAPP_KEY_9             = 57,
    PAPP_KEY_SEMICOLON     = 59,
    PAPP_KEY_EQUAL         = 61,
    PAPP_KEY_A             = 65,
    PAPP_KEY_B             = 66,
    PAPP_KEY_C             = 67,
    PAPP_KEY_D             = 68,
    PAPP_KEY_E             = 69,
    PAPP_KEY_F             = 70,
    PAPP_KEY_G             = 71,
    PAPP_KEY_H             = 72,
    PAPP_KEY_I             = 73,
    PAPP_KEY_J             = 74,
    PAPP_KEY_K             = 75,
    PAPP_KEY_L             = 76,
    PAPP_KEY_M             = 77,
    PAPP_KEY_N             = 78,
    PAPP_KEY_O             = 79,
    PAPP_KEY_P             = 80,
    PAPP_KEY_Q             = 81,
    PAPP_KEY_R             = 82,
    PAPP_KEY_S             = 83,
    PAPP_KEY_T             = 84,
    PAPP_KEY_U             = 85,
    PAPP_KEY_V             = 86,
    PAPP_KEY_W             = 87,
    PAPP_KEY_X             = 88,
    PAPP_KEY_Y             = 89,
    PAPP_KEY_Z             = 90,
    PAPP_KEY_LEFT_BRACKET  = 91,
    PAPP_KEY_BACKSLASH     = 92,
    PAPP_KEY_RIGHT_BRACKET = 93,
    PAPP_KEY_GRAVE_ACCENT  = 96,
    PAPP_KEY_WORLD_1       = 161,
    PAPP_KEY_WORLD_2       = 162 ,
    PAPP_KEY_ESCAPE        = 256,
    PAPP_KEY_ENTER         = 257,
    PAPP_KEY_TAB           = 258,
    PAPP_KEY_BACKSPACE     = 259,
    PAPP_KEY_INSERT        = 260,
    PAPP_KEY_DELETE        = 261,
    PAPP_KEY_RIGHT         = 262,
    PAPP_KEY_LEFT          = 263,
    PAPP_KEY_DOWN          = 264,
    PAPP_KEY_UP            = 265,
    PAPP_KEY_PAGE_UP       = 266,
    PAPP_KEY_PAGE_DOWN     = 267,
    PAPP_KEY_HOME          = 268,
    PAPP_KEY_END           = 269,
    PAPP_KEY_CAPS_LOCK     = 280,
    PAPP_KEY_SCROLL_LOCK   = 281,
    PAPP_KEY_NUM_LOCK      = 282,
    PAPP_KEY_PRINT_SCREEN  = 283,
    PAPP_KEY_PAUSE         = 284,
    PAPP_KEY_F1            = 290,
    PAPP_KEY_F2            = 291,
    PAPP_KEY_F3            = 292,
    PAPP_KEY_F4            = 293,
    PAPP_KEY_F5            = 294,
    PAPP_KEY_F6            = 295,
    PAPP_KEY_F7            = 296,
    PAPP_KEY_F8            = 297,
    PAPP_KEY_F9            = 298,
    PAPP_KEY_F10           = 299,
    PAPP_KEY_F11           = 300,
    PAPP_KEY_F12           = 301,
    PAPP_KEY_F13           = 302,
    PAPP_KEY_F14           = 303,
    PAPP_KEY_F15           = 304,
    PAPP_KEY_F16           = 305,
    PAPP_KEY_F17           = 306,
    PAPP_KEY_F18           = 307,
    PAPP_KEY_F19           = 308,
    PAPP_KEY_F20           = 309,
    PAPP_KEY_F21           = 310,
    PAPP_KEY_F22           = 311,
    PAPP_KEY_F23           = 312,
    PAPP_KEY_F24           = 313,
    PAPP_KEY_F25           = 314,
    PAPP_KEY_KP_0          = 320,
    PAPP_KEY_KP_1          = 321,
    PAPP_KEY_KP_2          = 322,
    PAPP_KEY_KP_3          = 323,
    PAPP_KEY_KP_4          = 324,
    PAPP_KEY_KP_5          = 325,
    PAPP_KEY_KP_6          = 326,
    PAPP_KEY_KP_7          = 327,
    PAPP_KEY_KP_8          = 328,
    PAPP_KEY_KP_9          = 329,
    PAPP_KEY_KP_DECIMAL    = 330,
    PAPP_KEY_KP_DIVIDE     = 331,
    PAPP_KEY_KP_MULTIPLY   = 332,
    PAPP_KEY_KP_SUBTRACT   = 333,
    PAPP_KEY_KP_ADD        = 334,
    PAPP_KEY_KP_ENTER      = 335,
    PAPP_KEY_KP_EQUAL      = 336,
    PAPP_KEY_LEFT_SHIFT    = 340,
    PAPP_KEY_LEFT_CONTROL  = 341,
    PAPP_KEY_LEFT_ALT      = 342,
    PAPP_KEY_LEFT_SUPER    = 343,
    PAPP_KEY_RIGHT_SHIFT   = 344,
    PAPP_KEY_RIGHT_CONTROL = 345,
    PAPP_KEY_RIGHT_ALT     = 346,
    PAPP_KEY_RIGHT_SUPER   = 347,
    PAPP_KEY_MENU          = 348,
    PAPP_KEY_COUNT         = 349,
} papp_key;

typedef enum papp_gamepad_button
{
    PAPP_BUTTON_ACTION_DOWN  = 0,
    PAPP_BUTTON_ACTION_RIGHT = 1,
    PAPP_BUTTON_ACTION_LEFT  = 2,
    PAPP_BUTTON_ACTION_UP    = 3,
    PAPP_BUTTON_LEFT_BUMPER  = 4,
    PAPP_BUTTON_RIGHT_BUMPER = 5,
    PAPP_BUTTON_BACK         = 6,
    PAPP_BUTTON_START        = 7,
    PAPP_BUTTON_GUIDE        = 8,
    PAPP_BUTTON_LEFT_THUMB   = 9,
    PAPP_BUTTON_RIGHT_THUMB  = 10,
    PAPP_BUTTON_DPAD_UP      = 11,
    PAPP_BUTTON_DPAD_RIGHT   = 12,
    PAPP_BUTTON_DPAD_DOWN    = 13,
    PAPP_BUTTON_DPAD_LEFT    = 14,
    PAPP_BUTTON_COUNT        = 15,

    PAPP_BUTTON_CROSS = PAPP_BUTTON_ACTION_DOWN,
    PAPP_BUTTON_CIRCLE = PAPP_BUTTON_ACTION_RIGHT,
    PAPP_BUTTON_SQUARE = PAPP_BUTTON_ACTION_LEFT,
    PAPP_BUTTON_TRIANGLE = PAPP_BUTTON_ACTION_UP,

    PAPP_BUTTON_A = PAPP_BUTTON_ACTION_DOWN,
    PAPP_BUTTON_B = PAPP_BUTTON_ACTION_RIGHT,
    PAPP_BUTTON_X = PAPP_BUTTON_ACTION_LEFT,
    PAPP_BUTTON_Y = PAPP_BUTTON_ACTION_UP,

} papp_gamepad_button;

typedef enum papp_gamepad_axis
{
    PAPP_AXIS_LEFT_X        = 0,
    PAPP_AXIS_LEFT_Y        = 1,
    PAPP_AXIS_RIGHT_X       = 2,
    PAPP_AXIS_RIGHT_Y       = 3,
    PAPP_AXIS_LEFT_TRIGGER  = 4,
    PAPP_AXIS_RIGHT_TRIGGER = 5,
    PAPP_AXIS_COUNT         = 6,
} papp_gamepad_axis;

typedef struct papp_texture {
    int width;
    int height;

    int padded_width;    // Power of two width (PROCYON_PSP only)
    int padded_height;   // Power of two height (PROCYON_PSP only)
    bool swizzled;       // Swizzle texture reads (PROCYON_PSP only)
    void *tex_data;

    union {
        unsigned int id; // OpenGL texture id (PROCYON_DESKTOP only)
        void *data;      // Texture data (PROCYON_PSP only)
    };
} papp_texture;

typedef struct papp_render_target {
    papp_texture texture;
    void *edram_offset;
} papp_render_target;

typedef struct papp_mat4
{
    float elements[4][4];
} papp_mat4;

#define PROCYON_ARRAY_COUNT(arr) (sizeof(arr)/sizeof((arr)[0]))

#ifdef __cplusplus
extern "C" {
#endif

void papp_init(int width, int height, const char *title);
void papp_terminate();
bool papp_should_close();

void papp_start_frame();
void papp_end_frame();
void papp_set_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void papp_clear();

bool papp_key_down(papp_key key);
bool papp_key_pressed(papp_key key);
bool papp_key_released(papp_key key);

bool papp_button_down(papp_gamepad_button button);
bool papp_button_pressed(papp_gamepad_button button);
bool papp_button_released(papp_gamepad_button button);
float papp_axis(papp_gamepad_axis axis);

papp_texture papp_load_texture(const char *path);
void papp_draw_texture(papp_texture texture, float x, float y);
void papp_draw_texture_rect(papp_texture texture, papp_rect source, papp_rect dest);
void papp_draw_texture_ex(papp_texture texture, papp_rect source, papp_rect dest, papp_vec2 origin, float rotation, papp_color tint);

papp_render_target papp_create_render_target(int width, int height);
void papp_enable_render_target(papp_render_target *render_target);
void papp_disable_render_target(void *temp_fb);

unsigned int papp_closest_greater_pow2(const unsigned int value);
papp_mat4 papp_ortho(float left, float right, float bottom, float top, float near, float far);

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // PROCYON_H