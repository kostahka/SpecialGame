#pragma once

#include "box2d/b2_body.h"
class physics_object
{
public:
    virtual b2Body& get_physics_body() = 0;

    explicit physics_object(bool static_rotation = false);
    virtual ~physics_object();

    bool   static_rotation;
    b2Vec2 direction_to_planet;
};