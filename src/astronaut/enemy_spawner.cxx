#include "astronaut/enemy_spawner.hxx"

#include "physics/physics.hxx"
#include "render/game-gui.hxx"

constexpr int enemy_min_spawn_time = 2000;
constexpr int enemy_max_spawn_time = 8000;

constexpr int wave_min_time = 10000;
constexpr int wave_max_time = 15000;

constexpr int wave_enemy_min_count = 6;
constexpr int wave_enemy_max_count = 20;

constexpr float spawn_min_angle = 3.14f / 9.f;
constexpr float spawn_max_angle = 3.14f / 4.f;

constexpr int target_id = 0;
constexpr int enemy_id  = 1;

enemy_spawner::enemy_spawner()
    : target(nullptr)
    , spawn_time(0)
    , killed_enemies(0)
    , wave_time(0)
    , total_wave_time(0)
    , wave_active(false)
    , wave_enemies_count(0)
    , wave_enemies_left_count(0)
    , total_wave_enemies_count(0)
{
}

void enemy_spawner::update(std::chrono::duration<int, std::milli> delta_time)
{
    if (target)
    {
        if (wave_active)
        {
            wave_active = wave_enemies_left_count > 0;
            if (wave_enemies_count > 0)
            {
                spawn_time -= delta_time.count();
                if (spawn_time < 0)
                {
                    spawn_time = get_random_spawn_time();

                    float target_angle =
                        physics::land.get_angle_to(target->get_pos());
                    auto first_enemy =
                        new enemy(physics::land.get_spawn_place(
                                      target_angle + get_random_spawn_angle()),
                                  target);
                    auto second_enemy =
                        new enemy(physics::land.get_spawn_place(
                                      target_angle - get_random_spawn_angle()),
                                  target);
                    first_enemy->add_destroy_listener(this, enemy_id);
                    second_enemy->add_destroy_listener(this, enemy_id);
                    wave_enemies_count -= 2;
                }
            }
        }
        else
        {
            wave_time -= delta_time.count();
            if (wave_time < 0)
            {
                wave_enemies_count       = get_random_enemies_count();
                wave_enemies_left_count  = wave_enemies_count;
                total_wave_enemies_count = wave_enemies_count;
                spawn_time               = get_random_spawn_time();

                wave_active     = true;
                wave_time       = get_random_wave_time();
                total_wave_time = wave_time;
            }
        }
    }
}

void enemy_spawner::init(astronaut* p_target)
{
    target = p_target;
    target->add_destroy_listener(this, target_id);
    killed_enemies  = 0;
    wave_time       = get_random_wave_time();
    total_wave_time = wave_time;
    wave_active     = false;
}

void enemy_spawner::on_destroy(int object_id)
{
    switch (object_id)
    {
        case target_id:
            target = nullptr;

            break;
        case enemy_id:
            killed_enemies++;
            wave_enemies_left_count--;
            break;
        default:

            break;
    }
}

int enemy_spawner::get_killed_enemies() const
{
    return killed_enemies;
}
void enemy_spawner::imgui_render()
{
    if (wave_active)
    {
        gui::draw_enemies_count(wave_enemies_left_count,
                                total_wave_enemies_count);
    }
    else
    {
        gui::draw_wave_time(wave_time, total_wave_time);
    }
}

int enemy_spawner::get_random_wave_time()
{
    return wave_min_time +
           static_cast<int>((static_cast<float>(rand()) / RAND_MAX) *
                            (wave_max_time - wave_min_time));
}
int enemy_spawner::get_random_spawn_time()
{
    return enemy_min_spawn_time +
           static_cast<int>((static_cast<float>(rand()) / RAND_MAX) *
                            (enemy_max_spawn_time - enemy_min_spawn_time));
}
float enemy_spawner::get_random_spawn_angle()
{
    return spawn_min_angle + (static_cast<float>(rand()) / RAND_MAX) *
                                 (spawn_max_angle - spawn_min_angle);
}
int enemy_spawner::get_random_enemies_count()
{
    return wave_enemy_min_count +
           static_cast<int>(
               (static_cast<float>(rand()) / RAND_MAX) *
               (wave_enemy_max_count / 2 - wave_enemy_min_count / 2)) *
               2;
}
