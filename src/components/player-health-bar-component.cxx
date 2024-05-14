#include "components/player-health-bar-component.hxx"

player_health_bar_component::player_health_bar_component()
    : component(name)
{
}

std::size_t player_health_bar_component::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t player_health_bar_component::deserialize(std::istream& is)
{
    return 0;
}

std::size_t player_health_bar_component::serialize_size() const
{
    return 0;
}

bool player_health_bar_component::imgui_editable_render()
{
    return false;
}