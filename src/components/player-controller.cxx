#include "components/player-controller.hxx"

#include "imgui.h"

player_controller::player_controller()
    : component(name)
{
}

std::size_t player_controller::serialize(std::ostream& os) const
{
    std::size_t size = 0;

    return size;
}

std::size_t player_controller::deserialize(std::istream& is)
{
    std::size_t size = 0;

    return size;
}

std::size_t player_controller::serialize_size() const
{
    std::size_t size = 0;

    return size;
}

bool player_controller::imgui_editable_render()
{
    bool edited = false;

    ImGui::PushID(this);

    edited = edited || ImGui::DragInt("Gun reload time", &total_reload_time);

    ImGui::PopID();

    return edited;
}