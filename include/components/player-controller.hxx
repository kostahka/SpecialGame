#pragma once

#include "Kengine/components/component.hxx"

constexpr int player_reload_time = 200;

struct player_controller : public Kengine::component
{
    static constexpr auto name = "player_controller";

    player_controller();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    bool imgui_editable_render() override;

    int  total_reload_time = player_reload_time;
    int  reload_time       = 0;
    int  hp                = 100;
    int  selected_gun      = 1;
    bool dead              = false;
    bool drill             = false;
};