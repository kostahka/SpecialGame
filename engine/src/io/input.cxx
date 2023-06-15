#include "Kengine/io/input.hxx"

#include <array>

#include <SDL_keyboard.h>
#include <SDL_mouse.h>

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

bool key_pressed(key k)
{
    return k_pressed[static_cast<int>(k)];
};
} // namespace keyboard
} // namespace Kengine::input