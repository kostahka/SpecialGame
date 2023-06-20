#include "astronaut/enemy.hxx"
#include "physics/physics.hxx"

constexpr int astronaut_id        = 0;
constexpr int target_astronaut_id = 1;

constexpr float aim_distance        = 40.f;
constexpr float enemy_shooting_time = 2000.f;

enemy::enemy(const Kengine::transform2d& pos, astronaut* target_astronaut)
    : target_astronaut(target_astronaut)
    , shooting_time(enemy_shooting_time)
{
    enemy_astronaut = new astronaut(pos, true);
    enemy_astronaut->add_destroy_listener(this, astronaut_id);
    target_astronaut->add_destroy_listener(this, target_astronaut_id);
}

void enemy::on_event(Kengine::event::game_event e) {}

void enemy::control(std::chrono::duration<int, std::milli> delta_time)
{
    if (target_astronaut && enemy_astronaut)
    {
        Kengine::transform2d target_pos = target_astronaut->get_pos();
        Kengine::transform2d pos        = enemy_astronaut->get_pos();
        Kengine::transform2d aim_vector = target_pos - pos;
        float                aim_angle = std::atan2(aim_vector.y, aim_vector.x);
        enemy_astronaut->aim(aim_angle);

        if (target_pos.distance(pos) > aim_distance)
        {
            float target_angle  = physics::land.get_angle_to(target_pos);
            float current_angle = physics::land.get_angle_to(pos);

            if (target_angle < current_angle)
                enemy_astronaut->move(1);
            else
                enemy_astronaut->move(-1);
        }

        float target_land_distance = physics::land.get_distance_to(target_pos);
        float land_distance        = physics::land.get_distance_to(pos);

        if (target_land_distance > land_distance)
            enemy_astronaut->fly();

        shooting_time -= delta_time.count();
        if (shooting_time < 0)
        {
            shooting_time = enemy_shooting_time;
            enemy_astronaut->shoot();
        }
    }
}

void enemy::on_destroy(int object_id)
{
    switch (object_id)
    {
        case astronaut_id:
            destroy();
            enemy_astronaut = nullptr;
            break;
        case target_astronaut_id:
            target_astronaut = nullptr;
            break;
        default:
            break;
    }
}
