#pragma once

#include "astronaut.hxx"
#include "game-object/controller.hxx"

class player : public controller, public destroy_listener
{
public:
    explicit player(const Kengine::transform2d& pos);

    void on_event(Kengine::event::game_event e) override;
    void control(std::chrono::duration<int, std::milli> delta_time) override;

    void on_destroy(int object_id) override;

    void select_gun(int gun);

    [[nodiscard]] astronaut* get_astronaut() const;

    [[nodiscard]] int get_hp() const;
    [[nodiscard]] int get_selected_gun() const;

private:
    int        reload_time;
    int        hp;
    bool       dead;
    astronaut* player_astronaut;
    int        selected_gun;
    bool       drill;
};
