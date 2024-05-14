#include "components/game-over-menu-component.hxx"

game_over_menu_component::game_over_menu_component()
    : component(name)
{
}

std::size_t game_over_menu_component::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t game_over_menu_component::deserialize(std::istream& is)
{
    return 0;
}

std::size_t game_over_menu_component::serialize_size() const
{
    return 0;
}

bool game_over_menu_component::imgui_editable_render()
{
    return false;
}