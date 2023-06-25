#pragma once

#include "Kengine/transform3d.hxx"

namespace camera
{
void                 init();
void                 window_resize();
void                 look_at(const Kengine::transform2d& pos,
                             const Kengine::transform2d& up_vector);
Kengine::transform2d get_pos();
float                get_angle();
Kengine::transform2d screen_to_global(const Kengine::transform2d&);
Kengine::transform2d screen_to_local(const Kengine::transform2d&);
} // namespace camera