#include "components/gui-selectable-component.hxx"

#include "imgui.h"

gui_selectable_component::gui_selectable_component()
    : component(name)
{
}

std::size_t gui_selectable_component::serialize(std::ostream& os) const
{
    std::size_t size = 0;

    size += Kengine::serialization::write(os, default_uv.x);
    size += Kengine::serialization::write(os, default_uv.y);
    size += Kengine::serialization::write(os, default_uv.w);
    size += Kengine::serialization::write(os, default_uv.h);
    size += Kengine::serialization::write(os, hovered_uv.x);
    size += Kengine::serialization::write(os, hovered_uv.y);
    size += Kengine::serialization::write(os, hovered_uv.w);
    size += Kengine::serialization::write(os, hovered_uv.h);
    size += Kengine::serialization::write(os, selected_uv.x);
    size += Kengine::serialization::write(os, selected_uv.y);
    size += Kengine::serialization::write(os, selected_uv.w);
    size += Kengine::serialization::write(os, selected_uv.h);

    return size;
}

std::size_t gui_selectable_component::deserialize(std::istream& is)
{
    std::size_t size = 0;

    size += Kengine::serialization::read(is, default_uv.x);
    size += Kengine::serialization::read(is, default_uv.y);
    size += Kengine::serialization::read(is, default_uv.w);
    size += Kengine::serialization::read(is, default_uv.h);
    size += Kengine::serialization::read(is, hovered_uv.x);
    size += Kengine::serialization::read(is, hovered_uv.y);
    size += Kengine::serialization::read(is, hovered_uv.w);
    size += Kengine::serialization::read(is, hovered_uv.h);
    size += Kengine::serialization::read(is, selected_uv.x);
    size += Kengine::serialization::read(is, selected_uv.y);
    size += Kengine::serialization::read(is, selected_uv.w);
    size += Kengine::serialization::read(is, selected_uv.h);

    return size;
}

std::size_t gui_selectable_component::serialize_size() const
{
    std::size_t size = 0;

    size += Kengine::serialization::size(default_uv.x);
    size += Kengine::serialization::size(default_uv.y);
    size += Kengine::serialization::size(default_uv.w);
    size += Kengine::serialization::size(default_uv.h);
    size += Kengine::serialization::size(hovered_uv.x);
    size += Kengine::serialization::size(hovered_uv.y);
    size += Kengine::serialization::size(hovered_uv.w);
    size += Kengine::serialization::size(hovered_uv.h);
    size += Kengine::serialization::size(selected_uv.x);
    size += Kengine::serialization::size(selected_uv.y);
    size += Kengine::serialization::size(selected_uv.w);
    size += Kengine::serialization::size(selected_uv.h);

    return size;
}

bool gui_selectable_component::imgui_editable_render()
{
    bool edited = false;

    ImGui::PushID(this);

    edited =
        edited || ImGui::DragFloat4("Default uv", (float*)&default_uv, 0.01f);
    edited =
        edited || ImGui::DragFloat4("Hovered uv", (float*)&hovered_uv, 0.01f);
    edited =
        edited || ImGui::DragFloat4("Selected uv", (float*)&selected_uv, 0.01f);

    edited = edited || ImGui::Checkbox("Selected", &selected);

    ImGui::PopID();

    return edited;
}