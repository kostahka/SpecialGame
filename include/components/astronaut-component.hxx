#pragma once

#include "Kengine/components/animation-component.hxx"
#include "Kengine/components/component-info.hxx"
#include "Kengine/components/component.hxx"

#include "astronaut/gun.hxx"

constexpr float astronaut_hp         = 100;
constexpr float astronaut_move_speed = 0.1f;
constexpr float astronaut_fly_speed  = 0.2f;

struct astronaut_component : public Kengine::component
{
    static constexpr auto          name = "astronaut_component";
    static Kengine::component_info info;

    astronaut_component();
    astronaut_component(Kengine::scene* sc, entt::entity cur_entity);
    astronaut_component(Kengine::scene& sc,
                        entt::entity    ent,
                        entt::entity    walk_sound_entity,
                        entt::entity    fly_sound_entity);
    astronaut_component(astronaut_component& other,
                        Kengine::scene*      sc,
                        entt::entity         cur_entity,
                        entt::entity         walk_sound_entity,
                        entt::entity         fly_sound_entity,
                        entt::entity         hand_anchor_entity,
                        entt::entity         pistol_entity,
                        entt::entity         drill_entity);

    astronaut_component(astronaut_component& other);
    astronaut_component(astronaut_component&& other);

    astronaut_component& operator=(astronaut_component& other);
    astronaut_component& operator=(astronaut_component&& other);

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    bool imgui_editable_render() override;

    void shoot();

    entt::entity walk_sound_entity = entt::null;
    entt::entity fly_sound_entity  = entt::null;

    entt::entity hand_anchor_entity = entt::null;
    entt::entity pistol_entity      = entt::null;
    entt::entity drill_entity       = entt::null;

    Kengine::vec2 to_earth_center{};
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