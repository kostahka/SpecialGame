#pragma once

#include "Kengine/io/input.hxx"
#include <cstdint>

namespace Kengine::event
{
enum class type
{
    keyboard_event,
    mouse_button_event,

    quit,

    unknown
};
struct mouse_button_event
{
    input::mouse::button button;
    uint8_t              clicks;
    bool                 pressed;
    float                x;
    float                y;
};
struct keyboard_event
{
    input::keyboard::key key;
    bool                 pressed;
};
struct game_event
{
    type g_type;
    union
    {
        mouse_button_event mouse;
        keyboard_event     keyboard;
    };
};

} // namespace Kengine::event
