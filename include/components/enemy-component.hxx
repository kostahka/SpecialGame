#pragma once

#include "Kengine/components/component.hxx"

#include "entt/entt.hpp"

constexpr float enemy_aim_distance = 40.f;
constexpr int   enemy_reload_time  = 2000;

struct enemy_component : public Kengine::component
{
    static constexpr auto name = "enemy_component";

    enemy_component();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    bool imgui_editable_render() override;

    int   reload_time      = enemy_reload_time;
    int   reload_time_less = 0;
    float aim_distance     = enemy_aim_distance;
};