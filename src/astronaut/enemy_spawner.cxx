#include "astronaut/enemy_spawner.hxx"

#include "physics/physics.hxx"

constexpr int   enemy_spawn_time = 5000;
constexpr float spawn_angle      = 3.14f / 6.f;

constexpr int target_id = 0;

enemy_spawner::enemy_spawner()
    : target(nullptr)
    , spawn_time(0)
{
}

void enemy_spawner::update(std::chrono::duration<int, std::milli> delta_time)
{
    if (target)
    {
        spawn_time -= delta_time.count();
        if (spawn_time < 0)
        {
            spawn_time = enemy_spawn_time;

            float target_angle = physics::land.get_angle_to(target->get_pos());
            new enemy(physics::land.get_spawn_place(target_angle + spawn_angle),
                      target);
            new enemy(physics::land.get_spawn_place(target_angle - spawn_angle),
                      target);
        }
    }
}

void enemy_spawner::init(astronaut* p_target)
{
    target = p_target;
    target->add_destroy_listener(this, target_id);
}

void enemy_spawner::on_destroy(int object_id)
{
    if (object_id == target_id)
        target = nullptr;
}
