#include "components/enemies-progress-bar-component.hxx"

enemies_progress_bar_component::enemies_progress_bar_component()
    : component(name)
{
}

std::size_t enemies_progress_bar_component::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t enemies_progress_bar_component::deserialize(std::istream& is)
{
    return 0;
}

std::size_t enemies_progress_bar_component::serialize_size() const
{
    return 0;
}

bool enemies_progress_bar_component::imgui_editable_render()
{
    return false;
}