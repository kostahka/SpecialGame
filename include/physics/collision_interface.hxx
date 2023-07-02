#pragma once

#include "astronaut/gun.hxx"

class collision_interface
{
public:
    virtual void hurt(int damage) = 0;
    virtual bool hurt(float                       radius,
                      float                       damage,
                      const Kengine::transform2d& pos,
                      gun_type                    g) = 0;
};