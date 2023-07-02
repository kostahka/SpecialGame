#pragma once

#include "Kengine/audio/audio.hxx"
#include "Kengine/render/shader-program.hxx"
#include "Kengine/render/texture.hxx"

namespace resources
{
void init();

extern Kengine::texture_object* special_game_texture;
extern Kengine::texture_object* crosshairs_texture;
extern Kengine::texture_object* joystick_texture;

extern Kengine::audio::sound_buffer* main_theme_sound_buffer;
extern Kengine::audio::sound_buffer* shooting_sound_buffer;
extern Kengine::audio::sound_buffer* hurt_sound_buffer;
extern Kengine::audio::sound_buffer* fly_sound_buffer;
extern Kengine::audio::sound_buffer* walking_sound_buffer;
extern Kengine::audio::sound_buffer* ground_damage_sound_buffer;
extern Kengine::audio::sound_buffer* drill_start_sound_buffer;
extern Kengine::audio::sound_buffer* drill_shooting_sound_buffer;
extern Kengine::audio::sound_buffer* ground_drilling_sound_buffer;

extern Kengine::audio::sound_object* main_theme_sound;

extern Kengine::shader_program* drill_beam_program;

}; // namespace resources