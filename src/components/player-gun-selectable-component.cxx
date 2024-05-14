#include "components/player-gun-selectable-component.hxx"

#include "imgui.h"

player_gun_selectable_component::player_gun_selectable_component()
    : component(name)
{
}

std::size_t player_gun_selectable_component::serialize(std::ostream& os) const
{
    std::size_t size = 0;

    size += Kengine::serialization::write(os, gun);

    return size;
}

std::size_t player_gun_selectable_component::deserialize(std::istream& is)
{
    std::size_t size = 0;

    size += Kengine::serialization::read(is, gun);

    return size;
}

std::size_t player_gun_selectable_component::serialize_size() const
{
    std::size_t size = 0;

    size += Kengine::serialization::size(gun);

    return size;
}

static std::unordered_map<gun_type, const char*> gun_type_str{
    {gun_type::pistol, "Pistol"},
    { gun_type::drill, "Drill" }
};

bool player_gun_selectable_component::imgui_editable_render()
{
    bool edited = false;

    ImGui::PushID(this);

    auto        gun_type_str_it = gun_type_str.find(gun);
    const char* gun_str         = gun_type_str_it != gun_type_str.end()
                                      ? gun_type_str_it->second
                                      : "undefined";

    if (ImGui::BeginCombo("Gun type", gun_str))
    {
        for (auto& gun_type_str_it : gun_type_str)
        {
            if (ImGui::Selectable(gun_type_str_it.second))
            {
                gun    = gun_type_str_it.first;
                edited = true;
            }
        }

        ImGui::EndCombo();
    }

    ImGui::PopID();

    return edited;
}