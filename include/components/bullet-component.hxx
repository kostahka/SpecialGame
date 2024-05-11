#pragma once

#include "Kengine/components/component.hxx"

constexpr float bullet_velocity      = 100.f;
constexpr float bullet_damage        = 20.f;
constexpr float bullet_ground_damage = 0.01f;
constexpr float bullet_damage_radius = 3.f;
constexpr int   bullet_life_time     = 3000;

struct bullet_component : public Kengine::component
{
    static constexpr auto name = "bullet_component";

    bullet_component();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    bool imgui_editable_render() override;

    float velocity      = bullet_velocity;
    float damage        = bullet_damage;
    float ground_damage = bullet_ground_damage;
    float damage_radius = bullet_damage_radius;
    int   life_time     = bullet_life_time;

    float angle          = 0;
    int   life_time_less = life_time;
};