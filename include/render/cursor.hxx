#pragma once

#include "Kengine/render/sprite.hxx"

enum class cursor_type
{
    simple,
    shovel,
    attack
};

class cursor
{
public:
    cursor();
    void draw() const;
    void set_cursor(cursor_type);

private:
    Kengine::sprite aim_sprite;
};