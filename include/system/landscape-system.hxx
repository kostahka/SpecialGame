#pragma once

#include "astronaut/gun.hxx"

#include "components/damage-interface.hxx"

#include "Kengine/graphics/draw-primitives.hxx"
#include "Kengine/graphics/vertex-array.hxx"
#include "Kengine/graphics/vertex-buffer.hxx"
#include "Kengine/resources/audio-resource.hxx"
#include "Kengine/resources/material-resource.hxx"
#include "Kengine/resources/res-ptr.hxx"
#include "Kengine/system/system.hxx"
#include "Kengine/units/vector.hxx"

#include "box2d/box2d.h"

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

struct landscape_vertex
{
    Kengine::vec2 pos;
};

using ground_table =
    std::array<std::array<ground, ground_w_count>, ground_h_count>;
using landscape_vertices =
    std::array<landscape_vertex, ground_vertices_end_index>;

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

class landscape_system : public Kengine::system, public damage_interface
{
public:
    static constexpr auto name = "landscape_system";

    landscape_system(Kengine::scene&);
    ~landscape_system();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    void on_start(Kengine::scene&) override;
    void on_render(Kengine::scene&, int delta_ms) override;
    void on_update(Kengine::scene&, int delta_ms) override;

    void change_ground(float x, float y, float radius, float delta_value);
    void hurt(int damage) override;
    bool hurt(float                radius,
              float                damage,
              const Kengine::vec2& pos,
              gun_type             g) override;

    [[nodiscard]] Kengine::vec2 get_center() const;
    [[nodiscard]] Kengine::vec2 get_spawn_place(float angle) const;
    [[nodiscard]] float         get_angle_to(const Kengine::vec2& pos) const;
    [[nodiscard]] float         get_distance_to(const Kengine::vec2& pos) const;
    [[nodiscard]] b2Body*       get_body() const;

    float gravity_force = 0.1f;

private:
    void calculate_vertices();
    void calculate_indexes();
    void recalculate_horizontal_vertex(size_t x, size_t y);
    void recalculate_vertical_vertex(size_t x, size_t y);

    void set_indexes(size_t                  x,
                     size_t                  y,
                     const triangle_indexes& t0,
                     const triangle_indexes& t1 = { 0, 0, 0 },
                     const triangle_indexes& t2 = { 0, 0, 0 },
                     const triangle_indexes& t3 = { 0, 0, 0 });
    void calculate_cell_indexes(size_t x, size_t y);

    void set_vao_indexes(size_t x, size_t y, int count);

    void set_cell_shape(size_t x, size_t y, int count, ...);

    ground_table g_table;

    landscape_vertices l_vertices;
    landscape_indexes  l_indexes;

    b2Body*            l_body = nullptr;
    landscape_fixtures l_fixtures;

    Kengine::graphics::gl_render_primitive* l_lines = nullptr;

    std::shared_ptr<Kengine::graphics::vertex_element_array> vao = nullptr;
    std::shared_ptr<Kengine::graphics::element_buffer>       ebo = nullptr;
    std::shared_ptr<Kengine::graphics::vertex_buffer<landscape_vertex>> vbo =
        nullptr;

    Kengine::res_ptr<Kengine::audio_resource>    damage_audio = nullptr;
    Kengine::res_ptr<Kengine::material_resource> material     = nullptr;

    Kengine::scene& sc;
};
