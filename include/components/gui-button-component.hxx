#pragma once

#include "Kengine/components/component.hxx"
#include "Kengine/units/rect.hxx"

struct gui_button_component : public Kengine::component
{
    static constexpr auto name = "gui_button_component";

    gui_button_component();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    bool imgui_editable_render() override;

    Kengine::rect default_uv{ 0, 0, 1, 1 };
    Kengine::rect hovered_uv{ 0, 0, 1, 1 };
    Kengine::rect click_uv{ 0, 0, 1, 1 };
};