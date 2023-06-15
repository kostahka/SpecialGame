#pragma once

#include "landscape.hxx"
#include "physics/physic-object.hxx"

#include "Kengine/render/b2GLDraw.hxx"
#include <box2d/box2d.h>

namespace physics
{
extern landscape land;
extern b2World   physics_world;
extern b2GLDraw  physics_draw;

void init();

void update(std::chrono::duration<int, std::milli> delta_time);

void add_physic_object(physics_object* obj);
void remove_physic_object(physics_object* obj);
} // namespace physics