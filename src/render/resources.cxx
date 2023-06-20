#include "render/resources.hxx"

namespace resources
{
Kengine::texture_object* special_game_texture{ nullptr };
Kengine::texture_object* crosshairs_texture{ nullptr };

Kengine::audio::sound_buffer* main_theme_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* shooting_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* hurt_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* fly_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* walking_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* ground_damage_sound_buffer{ nullptr };

Kengine::audio::sound_object* main_theme_sound{ nullptr };

void init()
{
    crosshairs_texture   = Kengine::create_texture("./assets/crosshairs.png");
    special_game_texture = Kengine::create_texture("./assets/SpecialGame.png");

    main_theme_sound_buffer =
        Kengine::audio::create_sound_buffer("./assets/SpaceSoundtrack.wav");
    shooting_sound_buffer =
        Kengine::audio::create_sound_buffer("./assets/BlastShooting.wav");
    hurt_sound_buffer =
        Kengine::audio::create_sound_buffer("./assets/AstronautHurt.wav");
    fly_sound_buffer = Kengine::audio::create_sound_buffer("./assets/Fly.wav");
    walking_sound_buffer =
        Kengine::audio::create_sound_buffer("./assets/Walking.wav");
    ground_damage_sound_buffer =
        Kengine::audio::create_sound_buffer("./assets/GroundDamage.wav");

    main_theme_sound =
        Kengine::audio::create_sound_object(main_theme_sound_buffer);
};
}; // namespace resources