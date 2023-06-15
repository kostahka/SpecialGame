#pragma once

#include "Kengine/transform3d.hxx"
#include "color.hxx"
namespace Kengine
{
enum class primitive_type
{
    points,
    lines,
    triangles
};
struct gl_render_primitive
{
    virtual void create()                     = 0;
    virtual void destroy()                    = 0;
    virtual void vertex(const transform3d& pos,
                        const color&       col,
                        const float        size = 1) = 0;
    virtual void draw()                       = 0;
};

gl_render_primitive* create_primitive_render(primitive_type);
} // namespace Kengine