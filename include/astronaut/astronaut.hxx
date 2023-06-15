#pragma once

#include "physics/physic-object.hxx"

#include "Kengine/render/animation.hxx"
#include "game-object/game-object.hxx"

enum class gun
{
    pistol,
    drill
};

class astronaut : public physics_object, public game_object
{
public:
    astronaut(const Kengine::transform2d& pos, bool enemy = false);

    void move(int direction);
    void fly();

    void aim(float angle);
    void select_gun(gun);
    void shoot();

    void on_event(Kengine::event::game_event e) override;
    void input_process() override;
    void update(std::chrono::duration<int, std::milli> delta_time) override;
    void render(std::chrono::duration<int, std::milli> delta_time) override;
    void destroy() override;

    b2Body& get_physics_body() override;

    Kengine::transform2d get_pos();

private:
    Kengine::animation_controller astronaut_anim;
    b2Body*                       astronaut_body;

    Kengine::sprite  hand_sprite;
    Kengine::sprite  pistol_sprite;
    Kengine::sprite  drill_sprite;
    Kengine::sprite* current_gun_sprite;

    bool moving;
    int  move_direction;
    bool flying;

    float gun_angle;
    gun   current_gun;
};