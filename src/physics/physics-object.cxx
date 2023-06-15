#include "physics/physic-object.hxx"

#include "physics/physics.hxx"

physics_object::physics_object(bool static_rotation)
    : static_rotation(static_rotation)
{
    physics::add_physic_object(this);
}

physics_object::~physics_object()
{
    physics::remove_physic_object(this);
};
