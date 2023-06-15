#pragma once

#include "Kengine/transform3d.hxx"
#include "color.hxx"

#include <cstdint>
#include <vector>

namespace Kengine
{

struct vertex
{
    transform3d pos;
};

struct vertex_color
{
    transform3d pos;
    color       col;
};

struct vertex_text2d
{
    transform3d pos;
    transform2d text;
};

struct vertex_color_size
{
    transform3d pos;
    color       col;
    float       size;
};

using vertex2d_array      = std::vector<transform2d>;
using vertex_array        = std::vector<vertex>;
using vertex_color_array  = std::vector<vertex_color>;
using vertex_text2d_array = std::vector<vertex_text2d>;
using indexes_vector      = std::vector<uint32_t>;
} // namespace Kengine
