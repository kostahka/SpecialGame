#pragma once

#include "shader-program.hxx"
namespace Kengine
{
namespace e_resources
{
void init();

extern shader_program* global_sprite_program;
extern shader_program* local_sprite_program;
extern shader_program* points_program;
extern shader_program* lines_program;
} // namespace e_resources
} // namespace Kengine