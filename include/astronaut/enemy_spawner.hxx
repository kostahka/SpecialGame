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

private:
    astronaut* target;
    int        spawn_time;
};