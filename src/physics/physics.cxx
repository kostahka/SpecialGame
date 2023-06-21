#include "physics/physics.hxx"

namespace physics
{
constexpr int velocity_iterations = 10;
constexpr int position_iterations = 8;

landscape                    land;
b2World                      physics_world({ 0.0f, 0.0f });
b2GLDraw                     physics_draw;
std::vector<physics_object*> ph_objects;

void init()
{
    physics_draw.Create();
    uint32 debug_draw_flags = 0;
    // debug_draw_flags += b2Draw::e_aabbBit;
    // debug_draw_flags += b2Draw::e_centerOfMassBit;
    // debug_draw_flags += b2Draw::e_jointBit;
    // debug_draw_flags += b2Draw::e_pairBit;
    debug_draw_flags += b2Draw::e_shapeBit;
    physics_draw.SetFlags(debug_draw_flags);
    physics_world.SetDebugDraw(&physics_draw);
};

void update(std::chrono::duration<int, std::milli> delta_time)
{
    physics::physics_world.Step(
        delta_time.count() / 1000.f, velocity_iterations, position_iterations);

    for (int i = 0; i < ph_objects.size(); i++)
    {
        b2Body& body                = ph_objects[i]->get_physics_body();
        b2Vec2  direction_to_planet = land.get_center() - body.GetPosition();
        float   distance_to_planet  = direction_to_planet.Normalize();

        ph_objects[i]->direction_to_planet = direction_to_planet;

        if (ph_objects[i]->static_rotation)
        {
            float stand_angle =
                std::atan2(direction_to_planet.y, direction_to_planet.x);
            body.SetTransform(body.GetPosition(), (stand_angle + 3.14f / 2.f));
        }

        if (distance_to_planet < 10)
            distance_to_planet = 10;

        direction_to_planet *= land.gravity_force * delta_time.count();
        body.ApplyLinearImpulseToCenter(direction_to_planet, true);
    }
};

void add_physic_object(physics_object* obj)
{
    ph_objects.push_back(obj);
};

void remove_physic_object(physics_object* obj)
{
    ph_objects.erase(std::remove(ph_objects.begin(), ph_objects.end(), obj));
};

} // namespace physics
