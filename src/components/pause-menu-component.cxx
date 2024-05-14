#include "components/pause-menu-component.hxx"

pause_menu_component::pause_menu_component()
    : component(name)
{
}

std::size_t pause_menu_component::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t pause_menu_component::deserialize(std::istream& is)
{
    return 0;
}

std::size_t pause_menu_component::serialize_size() const
{
    return 0;
}

bool pause_menu_component::imgui_editable_render()
{
    return false;
}