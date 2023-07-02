#pragma once

#include "Kengine/audio/audio.hxx"
#include "Kengine/render/animation.hxx"
#include "Kengine/render/draw-primitives.hxx"

#include "game-object/game-object.hxx"
#include "gun.hxx"
#include "physics/collision_interface.hxx"
#include "physics/physic-object.hxx"

#include <cstdint>

class astronaut : public physics_object,
                  public game_object,
                  public collision_interface
{
public:
    explicit astronaut(const Kengine::transform2d& pos, bool enemy = false);

    void move(float direction);
    void fly();

    void aim(float angle);
    void select_gun(gun_type);
    void shoot();

    void hurt(int damage) override;
    bool hurt(float                       radius,
              float                       damage,
              const Kengine::transform2d& pos,
              gun_type                    g) override;

    void update(std::chrono::duration<int, std::milli> delta_time) override;
    void render(std::chrono::duration<int, std::milli> delta_time) override;

    void destroy() override;

    b2Body& get_physics_body() override;

    Kengine::transform2d get_pos() const;
    int                  get_hp() const;
    float                get_angle() const;

    ~astronaut() override;

private:
    class GroundRayCastCallback : public b2RayCastCallback
    {
    public:
        explicit GroundRayCastCallback(astronaut* astro);

        float ReportFixture(b2Fixture*    fixture,
                            const b2Vec2& point,
                            const b2Vec2& normal,
                            float         fraction) override;

    private:
        astronaut* astro;
    } ground_ray_cast_callback;

    class DrillRayCastCallback : public b2RayCastCallback
    {
    public:
        explicit DrillRayCastCallback(astronaut* astro);

        float ReportFixture(b2Fixture*    fixture,
                            const b2Vec2& point,
                            const b2Vec2& normal,
                            float         fraction) override;

        uintptr_t drill_collision_info;
        b2Vec2    drill_collision_point;
        float     drill_distance;

    private:
        astronaut* astro;
    } drill_ray_cast_callback;

    Kengine::animation_controller astronaut_anim;
    b2Body*                       astronaut_body;
    b2Fixture*                    legs_fixture;

    Kengine::sprite  hand_sprite;
    Kengine::sprite  pistol_sprite;
    Kengine::sprite  drill_sprite;
    Kengine::sprite  drill_beam_sprite;
    Kengine::sprite* current_gun_sprite;

    Kengine::audio::sound_object* shooting_sound;
    Kengine::audio::sound_object* hurt_sound;
    Kengine::audio::sound_object* fly_sound;
    Kengine::audio::sound_object* walking_sound;
    Kengine::audio::sound_object* drill_shooting_sound;
    Kengine::audio::sound_object* drill_start_sound;
    Kengine::audio::sound_object* ground_drilling_sound;

    bool  moving;
    float move_direction;
    bool  flying;

    bool drilling;

    float    gun_angle;
    gun_type current_gun;

    float hp;

    bool on_ground;

    bool enemy;

    Kengine::gl_render_primitive* d_lines;
};
