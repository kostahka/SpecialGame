#include "Kengine/io/input.hxx"
#include "Kengine/engine.hxx"
#include "imgui.h"

#include <array>

#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_touch.h>

namespace Kengine::input
{
namespace mouse
{

std::array<bool, buttons_count> b_pressed{ false, false, false, false, false };

float x;
float y;

void update()
{
    const Uint32 state = SDL_GetMouseState(&x, &y);
    for (int i = 0; i < buttons_count; i++)
    {
        b_pressed[i] = state & SDL_BUTTON(i + 1);
    }
}

bool button_pressed(button b)
{
    return b_pressed[static_cast<int>(b) - 1];
}

void set_button_pressed(button b, bool pressed)
{
    b_pressed[static_cast<int>(b) - 1] = pressed;
};

} // namespace mouse

namespace keyboard
{
std::array<bool, keys_count> k_pressed{};

void update()
{
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    for (int i = static_cast<int>(key::key_first); i < keys_count; i++)
    {
        k_pressed[i] = state[i];
    }
};

void set_key_pressed(key k, bool pressed)
{
    k_pressed[static_cast<int>(k)] = pressed;
};

bool key_pressed(key k)
{
    return k_pressed[static_cast<int>(k)];
};
} // namespace keyboard

namespace touch
{
// Not working, because SDL_GetTouchFinger await for finger index in array
// implemented in SDL3 and not fingerId. SDL3 has no declaration to get
// finger index
//
// bool get_touch_pos(int64_t touch_id, int64_t finger_id, float* x, float* y)
//{
//    SDL_Finger* finger = SDL_GetTouchFinger(touch_id, finger_id);
//    if (finger)
//    {
//        *x = finger->x *
//        engine::instance()->e_game->configuration.screen_width; *y =
//            finger->y *
//            engine::instance()->e_game->configuration.screen_height;
//        return true;
//    }
//    return false;
//}
} // namespace touch
} // namespace Kengine::input