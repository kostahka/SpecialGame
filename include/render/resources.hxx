#pragma once

#include "Kengine/audio/audio.hxx"
#include "Kengine/render/texture.hxx"

namespace resources
{
void init();

extern Kengine::texture_object*      special_game_texture;
extern Kengine::texture_object*      crosshairs_texture;
extern Kengine::audio::sound_buffer* main_theme_sound;

}; // namespace resources