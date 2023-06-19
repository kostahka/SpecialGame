#include "render/resources.hxx"

namespace resources
{
Kengine::texture_object* special_game_texture{ nullptr };
Kengine::texture_object* crosshairs_texture{ nullptr };

Kengine::audio::sound_buffer* main_theme_sound{ nullptr };

void init()
{
    resources::crosshairs_texture =
        Kengine::create_texture("./assets/crosshairs.png");
    resources::special_game_texture =
        Kengine::create_texture("./assets/SpecialGame.png");
    main_theme_sound =
        Kengine::audio::create_sound_buffer("./assets/SpaceSoundtrack.wav");
};
}; // namespace resources