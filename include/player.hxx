#pragma once

#include "box2d/box2d.h"
#include "sprite.hxx"
#include "transform3d.hxx"

class player
{
public:
    player(const Kengine::transform2d& pos);

    void update(std::chrono::duration<int, std::milli> delta_time);
    void draw() const;

    Kengine::transform2d get_pos() const;

private:
    Kengine::sprite player_sprite;
    b2Body*         player_body;
};
