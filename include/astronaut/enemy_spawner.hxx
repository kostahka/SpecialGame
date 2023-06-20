#pragma once

#include "enemy.hxx"

#include <chrono>
#include <ratio>

class enemy_spawner : public destroy_listener
{
public:
    enemy_spawner();
    void init(astronaut* target);
    void update(std::chrono::duration<int, std::milli> delta_time);

    void on_destroy(int object_id) override;

    [[nodiscard]] int get_killed_enemies() const;

    void imgui_render();

private:
    static int   get_random_wave_time();
    static int   get_random_spawn_time();
    static float get_random_spawn_angle();
    static int   get_random_enemies_count();

    int        killed_enemies;
    astronaut* target;

    bool wave_active;
    int  wave_time;
    int  total_wave_time;
    int  wave_enemies_count;
    int  wave_enemies_left_count;
    int  total_wave_enemies_count;
    int  spawn_time;
};