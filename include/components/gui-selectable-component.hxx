#pragma once

#include "Kengine/components/component.hxx"
#include "Kengine/units/rect.hxx"

struct gui_selectable_component : public Kengine::component
{
    static constexpr auto name = "gui_selectable_component";

    gui_selectable_component();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    bool imgui_editable_render() override;

    bool selected = false;

    Kengine::rect default_uv{ 0, 0, 1, 1 };
    Kengine::rect hovered_uv{ 0, 0, 1, 1 };
    Kengine::rect selected_uv{ 0, 0, 1, 1 };
};