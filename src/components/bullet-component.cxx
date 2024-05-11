#include "components/bullet-component.hxx"

#include "imgui.h"

bullet_component::bullet_component()
    : component(name)
{
}

std::size_t bullet_component::serialize(std::ostream& os) const
{
    std::size_t size = 0;

    size += Kengine::serialization::write(os, velocity);
    size += Kengine::serialization::write(os, damage);
    size += Kengine::serialization::write(os, ground_damage);
    size += Kengine::serialization::write(os, damage_radius);
    size += Kengine::serialization::write(os, life_time);

    return size;
}

std::size_t bullet_component::deserialize(std::istream& is)
{
    std::size_t size = 0;

    size += Kengine::serialization::read(is, velocity);
    size += Kengine::serialization::read(is, damage);
    size += Kengine::serialization::read(is, ground_damage);
    size += Kengine::serialization::read(is, damage_radius);
    size += Kengine::serialization::read(is, life_time);

    return size;
}

std::size_t bullet_component::serialize_size() const
{
    std::size_t size = 0;

    size += Kengine::serialization::size(velocity);
    size += Kengine::serialization::size(damage);
    size += Kengine::serialization::size(ground_damage);
    size += Kengine::serialization::size(damage_radius);
    size += Kengine::serialization::size(life_time);

    return size;
}

bool bullet_component::imgui_editable_render()
{
    bool edited = false;

    ImGui::PushID(this);

    edited = edited || ImGui::DragFloat("Velocity", &velocity, 0.01f);
    edited = edited || ImGui::DragFloat("Damage", &damage, 0.1f);
    edited = edited || ImGui::DragFloat("Ground damage", &ground_damage, 0.1f);
    edited = edited || ImGui::DragFloat("Damage radius", &damage_radius, 0.1f);
    edited = edited || ImGui::DragInt("Life time", &life_time);

    ImGui::PopID();

    return edited;
}