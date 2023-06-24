#pragma once

#include "Kengine/render/sprite.hxx"

class joystick
{
public:
    explicit joystick(const Kengine::transform2d pos);

    void draw();
    void on_event(const Kengine::event::game_event& event);

    float axis_x; // from -1 to 1
    float axis_y; // from -1 to 1
    float mod;
    bool  active;

private:
    Kengine::sprite center_sprite;
    Kengine::sprite circle_sprite;

    Kengine::transform2d pos;
    int64_t              touch_id;
    int64_t              finger_id;
};