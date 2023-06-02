#pragma once

#include "transform3d.hxx"
#include "vertex.hxx"
#include <array>
#include <cstddef>
#include <cstdint>

namespace Kengine
{

class static_vertex_array_object
{
public:
    virtual void draw_triangles(int count) = 0;
};

class vertex_array_object
{
public:
    virtual void draw_triangles(int count)          = 0;
    virtual void draw_triangles_elements(int count) = 0;

    virtual void set_vertices(transform2d* data, long offset, long size) = 0;
    virtual void set_indexes(uint32_t* data, long offset, long size)     = 0;
};

vertex_array_object* create_vao(const transform2d* vertices,
                                const size_t       v_count,
                                const uint32_t*    indexes,
                                const size_t       i_count);

vertex_array_object* create_vao(const vertex_text2d_array&);

static_vertex_array_object* create_static_vao(const vertex_color_array&);
static_vertex_array_object* create_static_vao(const vertex_text2d_array&);
static_vertex_array_object* create_static_vao(const vertex_array&);

} // namespace Kengine
