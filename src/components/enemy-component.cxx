#include "components/enemy-component.hxx"

#include "imgui.h"

enemy_component::enemy_component()
    : component(name)
{
}

std::size_t enemy_component::serialize(std::ostream& os) const
{
    std::size_t size = 0;

    size += Kengine::serialization::write(os, reload_time);
    size += Kengine::serialization::write(os, aim_distance);

    return size;
}

std::size_t enemy_component::deserialize(std::istream& is)
{
    std::size_t size = 0;

    size += Kengine::serialization::read(is, reload_time);
    size += Kengine::serialization::read(is, aim_distance);

    return size;
}

std::size_t enemy_component::serialize_size() const
{
    std::size_t size = 0;

    size += Kengine::serialization::size(reload_time);
    size += Kengine::serialization::size(aim_distance);

    return size;
}

bool enemy_component::imgui_editable_render()
{
    bool edited = false;
    ImGui::PushID(this);

    edited = edited || ImGui::DragInt("Reload time", &reload_time);
    edited = edited || ImGui::DragFloat("Aim distance", &aim_distance, 0.1f);

    ImGui::PopID();

    return edited;
}