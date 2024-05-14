#pragma once

#include "Kengine/components/component.hxx"

struct pause_menu_component : public Kengine::component
{
    static constexpr auto name = "pause_menu_component";

    pause_menu_component();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    bool imgui_editable_render() override;
};