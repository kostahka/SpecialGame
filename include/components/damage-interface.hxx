#pragma once

#include "Kengine/units/vector.hxx"

#include "astronaut/gun.hxx"

class damage_interface
{
public:
    virtual void hurt(int damage) = 0;
    virtual bool hurt(float                radius,
                      float                damage,
                      const Kengine::vec2& pos,
                      gun_type             g) = 0;
};