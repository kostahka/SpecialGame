#pragma once

#include "Kengine/transform3d.hxx"
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
    virtual ~vertex_array_object() = default;

    virtual void draw_triangles(int count)                         = 0;
    virtual void draw_triangles_elements(int count)                = 0;
    virtual void draw_triangles_elements(int count, size_t offset) = 0;
    virtual void draw_points(int count)                            = 0;
    virtual void draw_lines(int count)                             = 0;

    virtual void set_vertices(void* data, size_t offset, size_t size)    = 0;
    virtual void set_indexes(uint32_t* data, size_t offset, size_t size) = 0;
};

vertex_array_object* create_vao(const transform2d* vertices,
                                const size_t       v_count,
                                const uint32_t*    indexes,
                                const size_t       i_count);

vertex_array_object* create_vao(const vertex_color_size* vertices,
                                const size_t             v_count);

vertex_array_object* create_vao(const vertex_color* vertices,
                                const size_t        v_count);

vertex_array_object* create_vao(const vertex_text2d_array&);

static_vertex_array_object* create_static_vao(const vertex_color_array&);
static_vertex_array_object* create_static_vao(const vertex_text2d_array&);
static_vertex_array_object* create_static_vao(const vertex_array&);

} // namespace Kengine
