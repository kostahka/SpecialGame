#include "resources.hxx"

namespace resources
{
Kengine::texture_object* special_game_texture{ nullptr };
Kengine::texture_object* crosshairs_texture{ nullptr };
Kengine::texture_object* joystick_texture{ nullptr };

Kengine::audio::sound_buffer* main_theme_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* shooting_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* hurt_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* fly_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* walking_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* ground_damage_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* drill_start_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* drill_shooting_sound_buffer{ nullptr };
Kengine::audio::sound_buffer* ground_drilling_sound_buffer{ nullptr };

Kengine::audio::sound_object* main_theme_sound{ nullptr };

Kengine::shader_program* drill_beam_program{ nullptr };

void init()
{
    crosshairs_texture   = Kengine::create_texture("assets/crosshairs.png");
    special_game_texture = Kengine::create_texture("assets/SpecialGame.png");
    joystick_texture     = Kengine::create_texture("assets/joysticks.png");

    main_theme_sound_buffer =
        Kengine::audio::create_sound_buffer("assets/SpaceSoundtrack.wav");
    shooting_sound_buffer =
        Kengine::audio::create_sound_buffer("assets/BlastShooting.wav");
    hurt_sound_buffer =
        Kengine::audio::create_sound_buffer("assets/AstronautHurt.wav");
    fly_sound_buffer = Kengine::audio::create_sound_buffer("assets/Fly.wav");
    walking_sound_buffer =
        Kengine::audio::create_sound_buffer("assets/Walking.wav");
    ground_damage_sound_buffer =
        Kengine::audio::create_sound_buffer("assets/GroundDamage.wav");
    drill_start_sound_buffer =
        Kengine::audio::create_sound_buffer("assets/DrillStart.wav");
    drill_shooting_sound_buffer =
        Kengine::audio::create_sound_buffer("assets/DrillShooting.wav");
    ground_drilling_sound_buffer =
        Kengine::audio::create_sound_buffer("assets/GroundDrilling.wav");

    main_theme_sound =
        Kengine::audio::create_sound_object(main_theme_sound_buffer);

    drill_beam_program = Kengine::create_shader_program(
        "shaders/drill-beam-vertex.vert", "shaders/drill-beam-fragment.frag");
};
}; // namespace resources