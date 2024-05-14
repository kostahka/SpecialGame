#pragma once

#include "Kengine/components/component.hxx"

#include "astronaut/gun.hxx"

struct player_gun_selectable_component : public Kengine::component
{
    static constexpr auto name = "player_gun_selectable_component";

    player_gun_selectable_component();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    bool imgui_editable_render() override;

    gun_type gun = gun_type::pistol;
};