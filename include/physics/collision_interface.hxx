#pragma once

class collision_interface
{
public:
    virtual void Hurt(int damage)                      = 0;
    virtual void Hurt(float                       radius,
                      float                       damage,
                      const Kengine::transform2d& pos) = 0;
};