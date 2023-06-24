#pragma once

#include "Kengine/io/input.hxx"
#include <cstdint>

namespace Kengine::event
{
enum class type
{
    keyboard_event,
    mouse_button_event,
    touch_event,
    touch_move_event,
    window_resize,

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
struct touch_event
{
    int64_t touch_id;
    int64_t finger_id;
    float   x;
    float   y;
    bool    pressed;
};

struct game_event
{
    type g_type;
    union
    {
        mouse_button_event mouse;
        keyboard_event     keyboard;
        touch_event        touch;
    };
};

} // namespace Kengine::event
