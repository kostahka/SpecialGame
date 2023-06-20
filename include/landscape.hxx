#pragma once

#include "Kengine/audio/audio.hxx"
#include "Kengine/engine.hxx"
#include "Kengine/render/draw-primitives.hxx"
#include "Kengine/render/shader-program.hxx"
#include "Kengine/render/texture.hxx"
#include "Kengine/render/vertex-array-object.hxx"
#include "box2d/box2d.h"
#include "physics/collision_interface.hxx"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

struct ground
{
    float value;
};

constexpr int ground_w_count = 300;
constexpr int ground_h_count = 300;

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

constexpr int cells_count         = (ground_w_count - 1) * (ground_h_count - 1);
constexpr int max_triangles_count = cells_count * 4;

using landscape_fixtures = std::array<b2Fixture*, cells_count>;

using landscape_indexes = std::array<uint32_t, max_triangles_count * 3>;

struct triangle_indexes
{
    uint32_t i0;
    uint32_t i1;
    uint32_t i2;
};

class landscape : public collision_interface
{
public:
    landscape();

    void init();
    void draw() const;
    void change_ground(float x, float y, float radius, float delta_value);

    void Hurt(int damage) override;
    void Hurt(float                       radius,
              float                       damage,
              const Kengine::transform2d& pos) override;

    [[nodiscard]] Kengine::transform2d get_center() const;
    [[nodiscard]] Kengine::transform2d get_spawn_place(float angle) const;
    [[nodiscard]] float get_angle_to(const Kengine::transform2d& pos) const;
    [[nodiscard]] float get_distance_to(const Kengine::transform2d& pos) const;
    [[nodiscard]] b2Body* get_body() const;

    ~landscape();

    float gravity_force;

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

    void set_cell_shape(size_t x, size_t y, int count, ...);

    ground_table g_table;

    landscape_vertices            l_vertices;
    landscape_indexes             l_indexes;
    Kengine::vertex_array_object* vao;
    Kengine::shader_program*      program;
    Kengine::texture_object*      ground_texture;

    b2Body*            l_body;
    landscape_fixtures l_fixtures;

    Kengine::gl_render_primitive* l_lines;

    Kengine::audio::sound_object* damage_sound;
};
