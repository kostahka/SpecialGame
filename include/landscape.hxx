#pragma once

#include "shader-program.hxx"
#include "vertex-array-object.hxx"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

struct ground
{
    float value;
};

constexpr int   ground_w_count = 50;
constexpr int   ground_h_count = 50;
constexpr float cell_size      = 10.0f;

constexpr float ground_value = 0.5f;

constexpr size_t ground_vertices_index = 0;
constexpr size_t ground_horizontal_vertices_index =
    ground_w_count * ground_h_count;
constexpr size_t ground_vertical_vertices_index =
    ground_horizontal_vertices_index + (ground_w_count - 1) * ground_h_count;
constexpr size_t ground_vertices_end_index =
    ground_vertical_vertices_index + (ground_h_count - 1) * ground_w_count;

using ground_table =
    std::array<std::array<ground, ground_w_count>, ground_h_count>;
using landscape_vertices =
    std::array<Kengine::transform2d, ground_vertices_end_index>;

constexpr int cells_count         = (ground_w_count) * (ground_h_count - 1);
constexpr int max_triangles_count = cells_count * 4;

using landscape_indexes = std::array<uint32_t, max_triangles_count * 3>;

struct triangle_indexes
{
    uint32_t i0;
    uint32_t i1;
    uint32_t i2;
};

class landscape
{
public:
    landscape();

    void init();
    void draw() const;
    void change_ground(float x, float y, float radius, float delta_value);

    ~landscape();

private:
    void calculate_vertices();
    void calculate_indexes();
    void recalculate_vertices(size_t x, size_t y);
    void recalculate_indexes(size_t x, size_t y);

    void set_indexes(size_t                  x,
                     size_t                  y,
                     const triangle_indexes& t0,
                     const triangle_indexes& t1 = { 0, 0, 0 },
                     const triangle_indexes& t2 = { 0, 0, 0 },
                     const triangle_indexes& t3 = { 0, 0, 0 });
    void calculate_cell_indexes(size_t x, size_t y);

    void set_vao_vertices(size_t index);
    void set_vao_indexes(size_t x, size_t y);

    ground_table                  g_table;
    landscape_vertices            l_vertices;
    landscape_indexes             l_indexes;
    Kengine::vertex_array_object* vao;
    Kengine::shader_program*      program;
    int                           triangles_count;
};
