#pragma once

#include "astronaut.hxx"
#include "game-object/controller.hxx"

class enemy : public controller, public destroy_listener
{
public:
    explicit enemy(const Kengine::transform2d& pos,
                   astronaut*                  target_astronaut);

    void on_event(Kengine::event::game_event e) override;
    void control(std::chrono::duration<int, std::milli> delta_time) override;

    void on_destroy(int object_id) override;

private:
    astronaut* enemy_astronaut;
    astronaut* target_astronaut;

    float shooting_time;
};