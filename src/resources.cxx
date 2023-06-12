#include "resources.hxx"

namespace resources
{
Kengine::texture_object* player_texture{ nullptr };
Kengine::texture_object* crosshairs_texture{ nullptr };

void init()
{
    resources::player_texture =
        Kengine::create_texture("./assets/astronaut.png");
    resources::crosshairs_texture =
        Kengine::create_texture("./assets/crosshairs.png");
};
}; // namespace resources