#include "components/enemy-wave-text-component.hxx"

enemy_wave_text_component::enemy_wave_text_component()
    : component(name)
{
}

std::size_t enemy_wave_text_component::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t enemy_wave_text_component::deserialize(std::istream& is)
{
    return 0;
}

std::size_t enemy_wave_text_component::serialize_size() const
{
    return 0;
}

bool enemy_wave_text_component::imgui_editable_render()
{
    return false;
}