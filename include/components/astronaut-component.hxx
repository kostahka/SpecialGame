#pragma once

#include "Kengine/components/animation-component.hxx"
#include "Kengine/components/component-info.hxx"
#include "Kengine/components/component.hxx"

#include "astronaut/gun.hxx"
#include "components/damage-interface.hxx"

constexpr float astronaut_hp         = 100;
constexpr float astronaut_move_speed = 0.1f;
constexpr float astronaut_fly_speed  = 0.2f;

struct astronaut_component : public Kengine::component, public damage_interface
{
    static constexpr auto          name = "astronaut_component";
    static Kengine::component_info info;

    astronaut_component();
    astronaut_component(Kengine::scene* sc, entt::entity cur_entity);
    astronaut_component(astronaut_component& other,
                        Kengine::scene*      sc,
                        entt::entity         cur_entity,
                        entt::entity         walk_sound_entity,
                        entt::entity         fly_sound_entity,
                        entt::entity         hand_anchor_entity,
                        entt::entity         pistol_entity,
                        entt::entity         drill_entity,
                        entt::entity         hurt_sound_entity,
                        entt::entity         drill_beam_entity);

    astronaut_component(astronaut_component& other);
    astronaut_component(astronaut_component&& other);

    astronaut_component& operator=(astronaut_component& other);
    astronaut_component& operator=(astronaut_component&& other);

    void hurt(int damage) override;
    bool hurt(float                radius,
              float                damage,
              const Kengine::vec2& pos,
              gun_type             g) override;

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    bool imgui_editable_render() override;

    void shoot();
    void select_gun(gun_type g_type);

    entt::entity walk_sound_entity = entt::null;
    entt::entity fly_sound_entity  = entt::null;
    entt::entity hurt_sound_entity = entt::null;

    entt::entity hand_anchor_entity = entt::null;
    entt::entity pistol_entity      = entt::null;
    entt::entity drill_entity       = entt::null;

    entt::entity drill_beam_entity = entt::null;

    float         to_earth_center_distance = 0;
    Kengine::vec2 to_earth_center_norm{};
    float         angle_to_earth = 0;
    bool          moving         = false;
    float         move_direction = 0;

    bool flying   = false;
    bool drilling = false;

    float gun_angle = 0;

    bool on_ground = false;

    float    move_speed  = astronaut_move_speed;
    float    fly_speed   = astronaut_fly_speed;
    float    hp          = astronaut_hp;
    bool     enemy       = false;
    gun_type current_gun = gun_type::pistol;

    class GroundRayCastCallback : public b2RayCastCallback
    {
    public:
        explicit GroundRayCastCallback(astronaut_component& astro);

        float ReportFixture(b2Fixture*    fixture,
                            const b2Vec2& point,
                            const b2Vec2& normal,
                            float         fraction) override;

    private:
        astronaut_component& astro;

    } ground_ray_cast_callback;

    class DrillRayCastCallback : public b2RayCastCallback
    {
    public:
        explicit DrillRayCastCallback(astronaut_component& astro);

        float ReportFixture(b2Fixture*    fixture,
                            const b2Vec2& point,
                            const b2Vec2& normal,
                            float         fraction) override;

        uintptr_t drill_collision_info  = 0;
        b2Vec2    drill_collision_point = { 0, 0 };
        float     drill_distance        = 0;

    private:
        astronaut_component& astro;
    } drill_ray_cast_callback;

    inline void initialize(Kengine::scene* sc, entt::entity cur_entity)
    {
        this->sc             = sc;
        this->current_entity = cur_entity;
    };

    inline void initialize(Kengine::scene* sc) { this->sc = sc; };

    inline entt::entity get_current_entity() const { return current_entity; }

private:
    entt::entity    current_entity = entt::null;
    Kengine::scene* sc             = nullptr;
};