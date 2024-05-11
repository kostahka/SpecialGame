#include "system/landscape-system.hxx"

#include "external/PerlinNoise.hxx"
#include "game.hxx"
#include "resources/resources.hxx"

#include "Kengine/audio/audio.hxx"
#include "Kengine/components/transform-component.hxx"
#include "Kengine/log/log.hxx"
#include "Kengine/scene/scene.hxx"

#include <cmath>
#include <iostream>

constexpr float render_size   = 500.0;
constexpr float cell_size     = 3.0f;
constexpr float land_friction = 0.2f;
constexpr float planet_radius = ground_w_count * 0.3;

struct land_POD_vec
{
    float x;
    float y;
};

landscape_system::landscape_system(Kengine::scene& sc)
    : Kengine::system(name, Kengine::system_render_type)
    , g_table()
    , l_vertices()
    , l_indexes()
    , l_fixtures()
    , gravity_force(0.1f)
    , sc(sc)

{
    damage_audio = resources::landscape_damage_audio;
    if (damage_audio)
    {
        damage_audio->take_data();
    }
    material = resources::landscape_material;
    if (material)
    {
        material->take_data();
    }
}

landscape_system::~landscape_system()
{
    if (damage_audio)
    {
        damage_audio->free_data();
    }
    if (material)
    {
        material->free_data();
    }
}

float interpolate_ground(float g1, float g2)
{
    // return (g1 - ground_value) / (g1 - g2);
    float t = 0;
    if (g1 > g2)
        t = g1 + g2 - ground_value;
    else
        t = 1.0f - g1 - g2 + ground_value;
    if (t < 0.1f)
        t = 0.1f;
    if (t > 0.9f)
        t = 0.9f;

    return t;
}

std::size_t landscape_system::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t landscape_system::deserialize(std::istream& is)
{
    return 0;
}

std::size_t landscape_system::serialize_size() const
{
    return 0;
}

void landscape_system::on_start(Kengine::scene& sc)
{
    KENGINE_TRACE("Land initialize:");

    KENGINE_TRACE("Initialize ground values...");
    const auto seed = static_cast<siv::PerlinNoise::seed_type>(time(nullptr));
    const siv::PerlinNoise perlin{ seed };
    const float            map_fill = 0.55f;

    const size_t planet_x = ground_w_count / 2;
    const size_t planet_y = ground_h_count / 2;

    for (size_t y = 0; y < ground_h_count; y++)
    {
        for (size_t x = 0; x < ground_w_count; x++)
        {
            float delta =
                std::sqrt(static_cast<float>((planet_x - x) * (planet_x - x) +
                                             (planet_y - y) * (planet_y - y)));
            if (delta < planet_radius)
            {
                auto ground = static_cast<float>(
                    perlin.normalizedOctave2D_01(static_cast<float>(x) / 8.0,
                                                 static_cast<float>(y) / 8.0,
                                                 2,
                                                 0.3f));
                ground              = ground < ground_value
                                          ? ground / ground_value * map_fill
                                          : (ground / ground_value - 1) * (1 - map_fill) +
                                   ground_value;
                g_table[y][x].value = ground > ground_value ? 1.f : 0.f;
            }
            else
            {
                g_table[y][x].value = 0.1f;
            }
        }
    }

    KENGINE_TRACE("Initialize cross vertices...");
    for (size_t i = ground_vertices_index; i < ground_horizontal_vertices_index;
         i++)
    {
        size_t x      = (i - ground_vertices_index) % ground_w_count;
        size_t y      = (i - ground_vertices_index) / ground_w_count;
        l_vertices[i] = {
            {static_cast<float>(x) * cell_size,
             static_cast<float>(y) * cell_size}
        };
    }
    if (l_body)
    {
        KENGINE_TRACE("Destroy land body...");
        sc.get_world().DestroyBody(l_body);
    }

    KENGINE_TRACE("Initialize land body...");

    b2BodyDef land_body_def;
    land_body_def.position.Set(0, 0);
    land_body_def.userData.pointer =
        reinterpret_cast<uintptr_t>(static_cast<damage_interface*>(this));
    l_body = sc.get_world().CreateBody(&land_body_def);
    l_fixtures.fill(nullptr);

    KENGINE_TRACE("Initialize land vertices...");
    calculate_vertices();
    KENGINE_TRACE("Initialize land indexes...");
    calculate_indexes();

    if (!vao)
    {
        KENGINE_TRACE("Initialize vbo...");
        vbo = std::make_shared<
            Kengine::graphics::vertex_buffer<landscape_vertex>>();
        vbo->bind();
        vbo->allocate_vertices(l_vertices.data(), l_vertices.size(), true);
        vbo->add_attribute_pointer(Kengine::graphics::vertex_attribute_pointer(
            Kengine::graphics::g_float, 3, 0, sizeof(landscape_vertex)));

        KENGINE_TRACE("Initialize ebo...");
        ebo = std::make_shared<Kengine::graphics::element_buffer>();
        ebo->bind();
        ebo->allocate_indexes(
            l_indexes.data(), static_cast<uint32_t>(l_indexes.size()), true);

        KENGINE_TRACE("Initialize vao...");
        vao = std::make_shared<Kengine::graphics::vertex_element_array>();
        vao->bind();
        vao->add_vertex_buffer(vbo);
        vao->set_elements(ebo);
    }
    else
    {
        KENGINE_TRACE("Reinitialize vbo...");
        vbo->set_vertices(
            l_vertices.data(), 0, static_cast<uint32_t>(l_vertices.size()));
        KENGINE_TRACE("Reinitialize ebo...");
        ebo->set_indexes(
            l_indexes.data(), 0, static_cast<uint32_t>(l_indexes.size()));
    }
    if (!l_lines)
    {
        KENGINE_TRACE("Initialize render lines...");
        l_lines = Kengine::graphics::create_primitive_render(
            Kengine::graphics::primitive_type::lines);
        l_lines->create();
    }

    KENGINE_TRACE("Finish initialize land.");
}

void landscape_system::on_render(Kengine::scene& sc, int delta_ms)
{
    Kengine::graphics::update_matrices(sc.get_camera().get_projection(),
                                       sc.get_camera().view);
    Kengine::graphics::bind_material(material);

    Kengine::vec2 camera_pos = { 0, 0 };
    auto          cam_ent    = sc.get_camera_entity();
    if (cam_ent != entt::null &&
        sc.registry.any_of<Kengine::transform_component>(cam_ent))
    {
        auto& cam_trans =
            sc.registry.get<Kengine::transform_component>(cam_ent);
        camera_pos = cam_trans.get_world_transform().position;
    }
    else
    {
        auto& cam  = sc.get_camera();
        camera_pos = cam.get_position();
    }
    int y_start = static_cast<int>(
        std::roundf((camera_pos.y - render_size / 2) / cell_size));
    int y_end = static_cast<int>(
        std::roundf((camera_pos.y + render_size / 2) / cell_size));
    int x_start = static_cast<int>(
        std::roundf((camera_pos.x - render_size / 2) / cell_size));
    int x_end = static_cast<int>(
        std::roundf((camera_pos.x + render_size / 2) / cell_size));

    if (y_end < 0 || x_end < 0 || x_start > ground_w_count - 2 ||
        y_start > ground_h_count - 2)
        return;
    if (y_start < 0)
        y_start = 0;
    if (x_start < 0)
        x_start = 0;
    if (y_end >= ground_h_count - 1)
        y_end = ground_h_count - 2;
    if (x_end >= ground_w_count - 1)
        x_end = ground_w_count - 2;

    for (int y = y_start; y <= y_end; y++)
    {
        int          count  = (x_end - x_start + 1) * 4 * 3;
        const size_t offset = (y * (ground_w_count - 1) + x_start) * 4 * 3;
        vao->bind();
        vao->draw(Kengine::graphics::draw_mode::triangles,
                  count,
                  static_cast<uint32_t>(offset));
    }

    // l_lines->vertex({ x_start * cell_size, y_start * cell_size, 0 },
    //                 { 1.f, 0.1f, 0.1f, 0.5f });
    // l_lines->vertex({ x_start * cell_size, (y_end + 1) * cell_size, 0 },
    //                 { 1.f, 0.1f, 0.1f, 0.5f });
    // l_lines->vertex({ x_start * cell_size, (y_end + 1) * cell_size, 0 },
    //                 { 1.f, 0.1f, 0.1f, 0.5f });
    // l_lines->vertex({ (x_end + 1) * cell_size, (y_end + 1) * cell_size, 0 },
    //                 { 1.f, 0.1f, 0.1f, 0.5f });
    // l_lines->vertex({ (x_end + 1) * cell_size, (y_end + 1) * cell_size, 0 },
    //                 { 1.f, 0.1f, 0.1f, 0.5f });
    // l_lines->vertex({ (x_end + 1) * cell_size, y_start * cell_size, 0 },
    //                 { 1.f, 0.1f, 0.1f, 0.5f });
    // l_lines->vertex({ (x_end + 1) * cell_size, y_start * cell_size, 0 },
    //                 { 1.f, 0.1f, 0.1f, 0.5f });
    // l_lines->vertex({ x_start * cell_size, y_start * cell_size, 0 },
    //                 { 1.f, 0.1f, 0.1f, 0.5f });
    auto          center_planet = get_center();
    Kengine::vec3 left_center   = { center_planet.x - 10, center_planet.y, 0 };
    Kengine::vec3 right_center  = { center_planet.x + 10, center_planet.y, 0 };
    Kengine::vec3 top_center    = { center_planet.x, center_planet.y + 10, 0 };
    Kengine::vec3 bottom_center = { center_planet.x, center_planet.y - 10, 0 };
    l_lines->vertex(left_center, { 0, 1, 1, 1 }, 5);
    l_lines->vertex(right_center, { 0, 1, 1, 1 }, 5);
    l_lines->vertex(top_center, { 0, 1, 1, 1 }, 5);
    l_lines->vertex(bottom_center, { 0, 1, 1, 1 }, 5);
    l_lines->draw();
    // vao->bind();
    // vao->draw(Kengine::graphics::draw_mode::triangles, l_indexes.size());
}

void landscape_system::on_update(Kengine::scene&, int delta_ms) {}

void landscape_system::change_ground(float x,
                                     float y,
                                     float radius,
                                     float delta_value)
{
    int y_start = (int)std::round((y - radius) / cell_size);
    int y_end   = (int)std::round((y + radius) / cell_size);

    if (y_start < 1)
        y_start = 1;
    if (y_end >= ground_h_count - 1)
        y_end = ground_h_count - 2;

    for (int gy = y_start; gy < y_end; gy++)
    {
        float r = (float)std::sqrt(
            std::abs(radius * radius - std::pow(y - gy * cell_size, 2)));

        int x_start = (int)std::round((x - r) / cell_size);
        int x_end   = (int)std::round((x + r) / cell_size);

        if (x_start < 1)
            x_start = 1;
        if (x_end >= ground_w_count - 1)
            x_end = ground_w_count - 2;

        for (int gx = x_start; gx <= x_end; gx++)
        {
            g_table[gy][gx].value += delta_value;
            if (g_table[gy][gx].value > 1.0)
                g_table[gy][gx].value = 1.0;
            if (g_table[gy][gx].value < 0.0)
                g_table[gy][gx].value = 0.0;
            recalculate_horizontal_vertex(gx - 1, gy);
            recalculate_vertical_vertex(gx, gy - 1);
            calculate_cell_indexes(gx - 1, gy - 1);
        }
        recalculate_horizontal_vertex(x_end, gy);
        calculate_cell_indexes(x_end, gy - 1);

        vbo->bind();
        int horizontal_v_index = ground_horizontal_vertices_index +
                                 gy * (ground_w_count - 1) + x_start - 1;
        vbo->set_vertices(&l_vertices[horizontal_v_index],
                          horizontal_v_index,
                          x_end - x_start + 2);
        int vertical_v_index = ground_vertical_vertices_index +
                               (gy - 1) * (ground_w_count) + x_start;
        vbo->set_vertices(&l_vertices[vertical_v_index],
                          vertical_v_index,
                          x_end - x_start + 1);
        set_vao_indexes(x_start - 1, gy - 1, x_end - x_start + 2);
    }
    {
        int gy = y_end;

        float r = (float)std::sqrt(
            std::abs(radius * radius - std::pow(y - gy * cell_size, 2)));

        int x_start = (int)std::round((x - r) / cell_size);
        int x_end   = (int)std::round((x + r) / cell_size);

        if (x_start < 1)
            x_start = 1;
        if (x_end >= ground_w_count - 1)
            x_end = ground_w_count - 2;

        for (int gx = x_start; gx <= x_end; gx++)
        {
            g_table[gy][gx].value += delta_value;
            if (g_table[gy][gx].value > 1.0)
                g_table[gy][gx].value = 1.0;
            if (g_table[gy][gx].value < 0.0)
                g_table[gy][gx].value = 0.0;
            recalculate_horizontal_vertex(gx - 1, gy);
            recalculate_vertical_vertex(gx, gy - 1);
            calculate_cell_indexes(gx - 1, gy - 1);
        }
        recalculate_horizontal_vertex(x_end, gy);
        calculate_cell_indexes(x_end, gy - 1);

        int horizontal_v_index = ground_horizontal_vertices_index +
                                 gy * (ground_w_count - 1) + x_start - 1;
        vbo->set_vertices(&l_vertices[horizontal_v_index],
                          horizontal_v_index,
                          x_end - x_start + 2);
        int vertical_v_index = ground_vertical_vertices_index +
                               (gy - 1) * (ground_w_count) + x_start;
        vbo->set_vertices(&l_vertices[vertical_v_index],
                          vertical_v_index,
                          x_end - x_start + 1);
        set_vao_indexes(x_start - 1, gy - 1, x_end - x_start + 2);
        for (int gx = x_start; gx <= x_end; gx++)
        {
            recalculate_vertical_vertex(gx, gy);
            calculate_cell_indexes(gx - 1, gy);
        }
        vertical_v_index =
            ground_vertical_vertices_index + (gy) * (ground_w_count) + x_start;
        vbo->set_vertices(&l_vertices[vertical_v_index],
                          vertical_v_index,
                          x_end - x_start + 1);
        set_vao_indexes(x_start - 1, gy, x_end - x_start + 2);
    }
};

void landscape_system::calculate_vertices()
{
    for (size_t i = ground_horizontal_vertices_index;
         i < ground_vertical_vertices_index;
         i++)
    {
        // getting x and y of vertices in horizontal lines.
        size_t x =
            (i - ground_horizontal_vertices_index) % (ground_w_count - 1);
        size_t y =
            (i - ground_horizontal_vertices_index) / (ground_w_count - 1);

        // getting vertex in horizontal line.
        float t =
            interpolate_ground(g_table[y][x].value, g_table[y][x + 1].value);
        l_vertices[i] = {
            {(x + t) * cell_size, y * cell_size}
        };
    }
    for (size_t i = ground_vertical_vertices_index;
         i < ground_vertices_end_index;
         i++)
    {
        // getting x and y of vertices on vertical lines.
        size_t x = (i - ground_vertical_vertices_index) % ground_w_count;
        size_t y = (i - ground_vertical_vertices_index) / ground_w_count;

        // getting vertex in vertical line.
        float t =
            interpolate_ground(g_table[y][x].value, g_table[y + 1][x].value);
        l_vertices[i] = {
            {x * cell_size, (y + t) * cell_size}
        };
    }
};

void landscape_system::set_indexes(size_t                  x,
                                   size_t                  y,
                                   const triangle_indexes& t0,
                                   const triangle_indexes& t1,
                                   const triangle_indexes& t2,
                                   const triangle_indexes& t3)
{
    const size_t cell_i     = (y * (ground_w_count - 1) + x) * 4 * 3;
    const size_t cell_index = y * (ground_w_count - 1) + x;

    l_indexes[cell_i + 0] = t0.i0;
    l_indexes[cell_i + 1] = t0.i1;
    l_indexes[cell_i + 2] = t0.i2;

    l_indexes[cell_i + 3] = t1.i0;
    l_indexes[cell_i + 4] = t1.i1;
    l_indexes[cell_i + 5] = t1.i2;

    l_indexes[cell_i + 6] = t2.i0;
    l_indexes[cell_i + 7] = t2.i1;
    l_indexes[cell_i + 8] = t2.i2;

    l_indexes[cell_i + 9]  = t3.i0;
    l_indexes[cell_i + 10] = t3.i1;
    l_indexes[cell_i + 11] = t3.i2;
}

void landscape_system::calculate_cell_indexes(size_t x, size_t y)
{
    uint8_t code = 0;
    if (g_table[y][x].value > ground_value)
        code += 0b0001;
    if (g_table[y][x + 1].value > ground_value)
        code += 0b0010;
    if (g_table[y + 1][x + 1].value > ground_value)
        code += 0b0100;
    if (g_table[y + 1][x].value > ground_value)
        code += 0b1000;

    const uint32_t v_i0 = (int)x + (int)y * ground_w_count;
    const uint32_t v_i1 = (int)x + 1 + (int)y * ground_w_count;
    const uint32_t v_i2 = (int)x + 1 + ((int)y + 1) * ground_w_count;
    const uint32_t v_i3 = (int)x + ((int)y + 1) * ground_w_count;

    const uint32_t v_hi0 = ground_horizontal_vertices_index + (int)x +
                           (int)y * (ground_w_count - 1);
    const uint32_t v_hi1 = ground_horizontal_vertices_index + (int)x +
                           ((int)y + 1) * (ground_w_count - 1);

    const uint32_t v_vi0 =
        ground_vertical_vertices_index + (int)x + (int)y * ground_w_count;
    const uint32_t v_vi1 =
        ground_vertical_vertices_index + (int)x + 1 + (int)y * ground_w_count;

    const b2Vec2 v0 = l_vertices[v_i0].pos;
    const b2Vec2 v1 = l_vertices[v_i1].pos;
    const b2Vec2 v2 = l_vertices[v_i2].pos;
    const b2Vec2 v3 = l_vertices[v_i3].pos;

    const b2Vec2 vh0 = l_vertices[v_hi0].pos;
    const b2Vec2 vh1 = l_vertices[v_hi1].pos;

    const b2Vec2 vv0 = l_vertices[v_vi0].pos;
    const b2Vec2 vv1 = l_vertices[v_vi1].pos;

    switch (code)
    {
        case 0b0001:
            set_cell_shape(x, y, 3, v0, vh0, vv0);
            set_indexes(x, y, { v_i0, v_hi0, v_vi0 });
            break;
        case 0b0010:
            set_cell_shape(x, y, 3, vv1, vh0, v1);
            set_indexes(x, y, { v_vi1, v_i1, v_hi0 });
            break;
        case 0b0100:
            set_cell_shape(x, y, 3, v2, vh1, vv1);
            set_indexes(x, y, { v_i2, v_hi1, v_vi1 });
            break;
        case 0b1000:
            set_cell_shape(x, y, 3, v3, vv0, vh1);
            set_indexes(x, y, { v_i3, v_vi0, v_hi1 });
            break;
        case 0b0011:
            set_cell_shape(x, y, 4, v0, v1, vv1, vv0);
            set_indexes(x, y, { v_i1, v_vi0, v_i0 }, { v_i1, v_vi1, v_vi0 });
            break;
        case 0b0110:
            set_cell_shape(x, y, 4, v1, v2, vh1, vh0);
            set_indexes(x, y, { v_i2, v_hi0, v_i1 }, { v_i2, v_hi1, v_hi0 });
            break;
        case 0b1100:
            set_cell_shape(x, y, 4, v2, v3, vv0, vv1);
            set_indexes(x, y, { v_i3, v_vi1, v_i2 }, { v_i3, v_vi0, v_vi1 });
            break;
        case 0b1001:
            set_cell_shape(x, y, 4, v3, v0, vh0, vh1);
            set_indexes(x, y, { v_i0, v_hi1, v_i3 }, { v_i0, v_hi0, v_hi1 });
            break;
        case 0b1111:
            set_cell_shape(x, y, 4, v0, v1, v2, v3);
            set_indexes(x, y, { v_i0, v_i1, v_i2 }, { v_i2, v_i3, v_i0 });
            break;
        case 0b0111:
            set_cell_shape(x, y, 5, v0, v1, v2, vh1, vv0);
            set_indexes(x,
                        y,
                        { v_i0, v_i1, v_vi0 },
                        { v_i1, v_hi1, v_vi0 },
                        { v_i1, v_i2, v_hi1 });
            break;
        case 0b1110:
            set_cell_shape(x, y, 5, v1, v2, v3, vv0, vh0);
            set_indexes(x,
                        y,
                        { v_i1, v_i2, v_hi0 },
                        { v_i2, v_vi0, v_hi0 },
                        { v_i2, v_i3, v_vi0 });
            break;
        case 0b1101:
            set_cell_shape(x, y, 5, v2, v3, v0, vh0, vv1);
            set_indexes(x,
                        y,
                        { v_i2, v_i3, v_vi1 },
                        { v_i3, v_hi0, v_vi1 },
                        { v_i3, v_i0, v_hi0 });
            break;
        case 0b1011:
            set_cell_shape(x, y, 5, v3, v0, v1, vv1, vh1);
            set_indexes(x,
                        y,
                        { v_i3, v_i0, v_hi1 },
                        { v_i0, v_vi1, v_hi1 },
                        { v_i0, v_i1, v_vi1 });
            break;
        case 0b0101:
            set_cell_shape(x, y, 6, v0, vh0, vv1, v2, vh1, vv0);
            set_indexes(x,
                        y,
                        { v_i0, v_hi0, v_vi0 },
                        { v_i2, v_hi1, v_vi1 },
                        { v_hi0, v_hi1, v_vi0 },
                        { v_hi0, v_vi1, v_hi1 });
            break;
        case 0b1010:
            set_cell_shape(x, y, 6, v1, vv1, vh1, v3, vv0, vh0);
            set_indexes(x,
                        y,
                        { v_i1, v_vi1, v_hi0 },
                        { v_i3, v_vi0, v_hi1 },
                        { v_hi0, v_vi1, v_vi0 },
                        { v_vi1, v_hi1, v_vi0 });
            break;
        default:
            set_cell_shape(x, y, 0);
            set_indexes(x, y, { 0, 0, 0 });
    }
};

void landscape_system::calculate_indexes()
{
    for (size_t y = 0; y < ground_h_count - 1; y++)
    {
        for (size_t x = 0; x < ground_w_count - 1; x++)
        {
            calculate_cell_indexes(x, y);
        }
    }
};

void landscape_system::recalculate_horizontal_vertex(size_t x, size_t y)
{
    int index =
        (int)(ground_horizontal_vertices_index + y * (ground_w_count - 1) + x);

    float t = interpolate_ground(g_table[y][x].value, g_table[y][x + 1].value);
    l_vertices[index] = {
        {(x + t) * cell_size, y * cell_size}
    };
}

void landscape_system::recalculate_vertical_vertex(size_t x, size_t y)
{
    int index =
        (int)(ground_vertical_vertices_index + (y) * (ground_w_count) + x);

    float t = interpolate_ground(g_table[y][x].value, g_table[y + 1][x].value);
    l_vertices[index] = {
        {x * cell_size, (y + t) * cell_size}
    };
};

Kengine::vec2 landscape_system::get_center() const
{
    return { ground_w_count * cell_size / 2, ground_h_count * cell_size / 2 };
}

void landscape_system::set_cell_shape(size_t x, size_t y, int count, ...)
{
    va_list vertices_list;
    va_start(vertices_list, count);

    const size_t index = x + (ground_w_count - 1) * y;

    b2Fixture* old_fixture = l_fixtures[index];
    if (old_fixture)
    {
        l_body->DestroyFixture(old_fixture);
        l_fixtures[index] = 0;
    }

    if (count > 0)
    {
        b2Vec2* vertices = new b2Vec2[count];

        for (int i = 0; i < count; i++)
        {
            auto temp_vec = va_arg(vertices_list, land_POD_vec);
            vertices[i]   = { temp_vec.x, temp_vec.y };
        }

        b2ChainShape chain_shape;
        chain_shape.CreateLoop(vertices, count);

        l_fixtures[index] = l_body->CreateFixture(&chain_shape, 0.f);
        l_fixtures[index]->SetFriction(land_friction);

        delete[] vertices;
    }

    va_end(vertices_list);
}

void landscape_system::hurt(int damage) {}

bool landscape_system::hurt(float                radius,
                            float                damage,
                            const Kengine::vec2& pos,
                            gun_type             g)
{
    switch (g)
    {
        case gun_type::pistol:
            if (damage_audio)
            {
                Kengine::audio::play_one_shot(damage_audio, { 1.0f, 1.0f });
            }
            break;
        case gun_type::drill:
            break;
    }
    change_ground(pos.x, pos.y, radius, -damage);
    return true;
}

Kengine::vec2 landscape_system::get_spawn_place(float angle) const
{
    constexpr float spawn_radius = planet_radius * cell_size + 2 * cell_size;
    Kengine::vec2   center       = get_center();

    return { center.x + std::cos(angle) * spawn_radius,
             center.y + std::sin(angle) * spawn_radius };
}

float landscape_system::get_angle_to(const Kengine::vec2& pos) const
{
    Kengine::vec2 center_pos = get_center();
    Kengine::vec2 vec        = pos - center_pos;
    return std::atan2(vec.y, vec.x);
}

float landscape_system::get_distance_to(const Kengine::vec2& pos) const
{
    Kengine::vec2 center_pos = get_center();
    return static_cast<float>(glm::length(glm::vec2(center_pos - pos)));
}

b2Body* landscape_system::get_body() const
{
    return l_body;
}

void landscape_system::set_vao_indexes(size_t x, size_t y, int count)
{
    const size_t cell_i = (y * (ground_w_count - 1) + x) * 4 * 3;

    uint32_t* data = &l_indexes[cell_i];
    size_t    size = 4 * 3 * count;
    ebo->bind();
    ebo->set_indexes(
        data, static_cast<uint32_t>(cell_i), static_cast<uint32_t>(size));
}
