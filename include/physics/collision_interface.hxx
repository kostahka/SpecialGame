#pragma once

#include "astronaut/gun.hxx"

class collision_interface
{
public:
    virtual void Hurt(int damage) = 0;
    virtual void Hurt(float                       radius,
                      float                       damage,
                      const Kengine::transform2d& pos,
                      gun_type                    g) = 0;
};