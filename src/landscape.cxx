#include "landscape.hxx"

#include "Kengine/engine.hxx"
#include "Kengine/render/shader-program.hxx"
#include "Kengine/render/texture.hxx"
#include "Kengine/transform3d.hxx"

#include "external/PerlinNoise.hxx"
#include "game.hxx"
#include "physics/physics.hxx"
#include "render/camera.hxx"
#include "resources.hxx"

#include <cmath>
#include <iostream>

using namespace Kengine;

constexpr float render_size   = 500.0;
constexpr float cell_size     = 3.0f;
constexpr float land_friction = 0.2f;
constexpr float planet_radius = ground_w_count * 0.3;

landscape::landscape()
    : g_table()
    , l_vertices()
    , l_indexes()
    , l_fixtures()
    , vao(nullptr)
    , program(nullptr)
    , ground_texture(nullptr)
    , gravity_force(0.1f)
    , l_body(nullptr)
    , l_lines(nullptr)
    , damage_sound(nullptr)
{
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

void landscape::init()
{
    std::cout << "Land init:" << std::endl;

    std::cout << "Init ground values..." << std::endl;
    const siv::PerlinNoise::seed_type seed = time(nullptr);
    const siv::PerlinNoise            perlin{ seed };
    const float                       map_fill = 0.55;

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
                g_table[y][x].value = ground > ground_value ? 1 : 0;
            }
            else
            {
                g_table[y][x].value = 0.1;
            }
        }
    }

    std::cout << "Init cross vertices" << std::endl;
    for (size_t i = ground_vertices_index; i < ground_horizontal_vertices_index;
         i++)
    {
        size_t x      = (i - ground_vertices_index) % ground_w_count;
        size_t y      = (i - ground_vertices_index) / ground_w_count;
        l_vertices[i] = { static_cast<float>(x) * cell_size,
                          static_cast<float>(y) * cell_size };
    }
    if (l_body)
    {
        std::cout << "Destroy land body..." << std::endl;
        physics::physics_world.DestroyBody(l_body);
    }

    std::cout << "Init land body..." << std::endl;

    b2BodyDef land_body_def;
    land_body_def.position.Set(0, 0);
    land_body_def.userData.pointer =
        reinterpret_cast<uintptr_t>(static_cast<collision_interface*>(this));
    l_body = physics::physics_world.CreateBody(&land_body_def);
    l_fixtures.fill(nullptr);

    std::cout << "Init vertices..." << std::endl;
    calculate_vertices();
    std::cout << "Init indexes..." << std::endl;
    calculate_indexes();

    if (!vao)
    {
        std::cout << "Init vao..." << std::endl;
        vao = Kengine::create_vao(l_vertices.data(),
                                  l_vertices.size(),
                                  l_indexes.data(),
                                  l_indexes.size());
    }
    else
    {
        std::cout << "Reinit vao..." << std::endl;
        vao->set_vertices(
            l_vertices.data(), 0, l_vertices.size() * sizeof(transform2d));
        vao->set_indexes(
            l_indexes.data(), 0, l_indexes.size() * sizeof(uint32_t));
    }
    if (!program)
    {
        std::cout << "Init shader program..." << std::endl;
        program = create_shader_program("shaders/landscape-vertex.vert",
                                        "shaders/landscape-fragment.frag");
    }
    if (!ground_texture)
    {
        std::cout << "Init texture..." << std::endl;
        ground_texture = create_texture("assets/ground.png");
    }
    if (!l_lines)
    {
        std::cout << "Init render lines..." << std::endl;
        l_lines = create_primitive_render(Kengine::primitive_type::lines);
        l_lines->create();
    }
    if (!damage_sound)
    {
        damage_sound = Kengine::audio::create_sound_object(
            resources::ground_damage_sound_buffer);
    }
    std::cout << "End init land." << std::endl;
}

landscape::~landscape()
{
    delete program;
}

void landscape::draw() const
{
    ground_texture->bind();
    program->use();
    program->set_uniform_matrix4fv("projection", current_game->projection);
    program->set_uniform_matrix4fv("view", current_game->view);

    transform2d camera_pos = camera::get_pos();
    int         y_start    = static_cast<int>(
        std::roundf((camera_pos.y - render_size / 2) / cell_size));
    int y_end = static_cast<int>(
        std::roundf((camera_pos.y + render_size / 2) / cell_size));
    int x_start = static_cast<int>(
        std::roundf((camera_pos.x - render_size / 2) / cell_size));
    int x_end = static_cast<int>(
        std::roundf((camera_pos.x + render_size / 2) / cell_size));

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
        int          count = (x_end - x_start + 1) * 4 * 3;
        const size_t offset =
            (y * (ground_w_count - 1) + x_start) * 4 * 3 * sizeof(uint32_t);
        vao->draw_triangles_elements(count, offset);
    }

    //    l_lines->vertex({ x_start * cell_size, y_start * cell_size, 0 },
    //                    { 1.f, 0.1f, 0.1f, 0.5f });
    //    l_lines->vertex({ x_start * cell_size, (y_end + 1) * cell_size, 0 },
    //                    { 1.f, 0.1f, 0.1f, 0.5f });
    //    l_lines->vertex({ x_start * cell_size, (y_end + 1) * cell_size, 0 },
    //                    { 1.f, 0.1f, 0.1f, 0.5f });
    //    l_lines->vertex({ (x_end + 1) * cell_size, (y_end + 1) * cell_size, 0
    //    },
    //                    { 1.f, 0.1f, 0.1f, 0.5f });
    //    l_lines->vertex({ (x_end + 1) * cell_size, (y_end + 1) * cell_size, 0
    //    },
    //                    { 1.f, 0.1f, 0.1f, 0.5f });
    //    l_lines->vertex({ (x_end + 1) * cell_size, y_start * cell_size, 0 },
    //                    { 1.f, 0.1f, 0.1f, 0.5f });
    //    l_lines->vertex({ (x_end + 1) * cell_size, y_start * cell_size, 0 },
    //                    { 1.f, 0.1f, 0.1f, 0.5f });
    //    l_lines->vertex({ x_start * cell_size, y_start * cell_size, 0 },
    //                    { 1.f, 0.1f, 0.1f, 0.5f });
    //    l_lines->draw();
    // vao->draw_triangles_elements(l_indexes.size());
};

void landscape::change_ground(float x, float y, float radius, float delta_value)
{
    int y_start = std::round((y - radius) / cell_size);
    int y_end   = std::round((y + radius) / cell_size);

    if (y_start < 1)
        y_start = 1;
    if (y_end >= ground_h_count - 1)
        y_end = ground_h_count - 2;

    for (int gy = y_start; gy < y_end; gy++)
    {
        float r = std::sqrt(
            std::abs(radius * radius - std::pow(y - gy * cell_size, 2)));

        int x_start = std::round((x - r) / cell_size);
        int x_end   = std::round((x + r) / cell_size);

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
            recalculate_indexes(gx - 1, gy - 1);
        }
        recalculate_horizontal_vertex(x_end, gy);
        recalculate_indexes(x_end, gy - 1);
    }
    {
        int gy = y_end;

        float r = std::sqrt(
            std::abs(radius * radius - std::pow(y - gy * cell_size, 2)));

        int x_start = std::round((x - r) / cell_size);
        int x_end   = std::round((x + r) / cell_size);

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
            recalculate_indexes(gx - 1, gy - 1);
        }
        recalculate_horizontal_vertex(x_end, gy);
        recalculate_indexes(x_end, gy - 1);
        for (int gx = x_start; gx <= x_end; gx++)
        {
            recalculate_vertical_vertex(gx, gy);
            recalculate_indexes(gx - 1, gy);
        }
    }
};

void landscape::calculate_vertices()
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
        l_vertices[i] = { (x + t) * cell_size, y * cell_size };
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
        l_vertices[i] = { x * cell_size, (y + t) * cell_size };
    }
};

void landscape::set_indexes(size_t                  x,
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

void landscape::calculate_cell_indexes(size_t x, size_t y)
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

    const uint32_t v_i0 = x + y * ground_w_count;
    const uint32_t v_i1 = x + 1 + y * ground_w_count;
    const uint32_t v_i2 = x + 1 + (y + 1) * ground_w_count;
    const uint32_t v_i3 = x + (y + 1) * ground_w_count;

    const uint32_t v_hi0 =
        ground_horizontal_vertices_index + x + y * (ground_w_count - 1);
    const uint32_t v_hi1 =
        ground_horizontal_vertices_index + x + (y + 1) * (ground_w_count - 1);

    const uint32_t v_vi0 =
        ground_vertical_vertices_index + x + y * ground_w_count;
    const uint32_t v_vi1 =
        ground_vertical_vertices_index + x + 1 + y * ground_w_count;

    const transform2d& v0 = l_vertices[v_i0];
    const transform2d& v1 = l_vertices[v_i1];
    const transform2d& v2 = l_vertices[v_i2];
    const transform2d& v3 = l_vertices[v_i3];

    const transform2d& vh0 = l_vertices[v_hi0];
    const transform2d& vh1 = l_vertices[v_hi1];

    const transform2d& vv0 = l_vertices[v_vi0];
    const transform2d& vv1 = l_vertices[v_vi1];

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

void landscape::calculate_indexes()
{
    for (size_t y = 0; y < ground_h_count - 1; y++)
    {
        for (size_t x = 0; x < ground_w_count - 1; x++)
        {
            calculate_cell_indexes(x, y);
        }
    }
};

void landscape::recalculate_horizontal_vertex(size_t x, size_t y)
{
    int index = ground_vertical_vertices_index + (y) * (ground_w_count) + x;

    float t = interpolate_ground(g_table[y][x].value, g_table[y + 1][x].value);
    l_vertices[index] = { x * cell_size, (y + t) * cell_size };
    set_vao_vertices(index);
}

void landscape::recalculate_vertical_vertex(size_t x, size_t y)
{
    int index = ground_horizontal_vertices_index + y * (ground_w_count - 1) + x;

    float t = interpolate_ground(g_table[y][x].value, g_table[y][x + 1].value);
    l_vertices[index] = { (x + t) * cell_size, y * cell_size };
    set_vao_vertices(index);
};

void landscape::recalculate_indexes(size_t x, size_t y)
{
    calculate_cell_indexes(x, y);
    set_vao_indexes(x, y);
};

void landscape::set_vao_indexes(size_t x, size_t y)
{
    const size_t cell_i = (y * (ground_w_count - 1) + x) * 4 * 3;

    uint32_t* data   = &l_indexes[cell_i];
    size_t    offset = cell_i * sizeof(uint32_t);
    size_t    size   = sizeof(uint32_t) * 4 * 3;
    vao->set_indexes(data, offset, size);
};

void landscape::set_vao_vertices(size_t index)
{
    transform2d* data   = &l_vertices[index];
    size_t       offset = index * sizeof(transform2d);
    size_t       size   = sizeof(transform2d);
    vao->set_vertices(data, offset, size);
}
Kengine::transform2d landscape::get_center() const
{
    return { ground_w_count * cell_size / 2, ground_h_count * cell_size / 2 };
}
void landscape::set_cell_shape(size_t x, size_t y, int count, ...)
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
            vertices[i] = va_arg(vertices_list, transform2d);
        }

        b2ChainShape chain_shape;
        chain_shape.CreateLoop(vertices, count);

        l_fixtures[index] = l_body->CreateFixture(&chain_shape, 0.f);
        l_fixtures[index]->SetFriction(land_friction);

        delete[] vertices;
    }

    va_end(vertices_list);
}
void landscape::hurt(int damage) {}
bool landscape::hurt(float                       radius,
                     float                       damage,
                     const Kengine::transform2d& pos,
                     gun_type                    g)
{
    switch (g)
    {
        case gun_type::pistol:
            damage_sound->play();
            break;
        case gun_type::drill:
            break;
    }
    change_ground(pos.x, pos.y, radius, -damage);
    return true;
}
Kengine::transform2d landscape::get_spawn_place(float angle) const
{
    constexpr float spawn_radius = planet_radius * cell_size + 2 * cell_size;
    transform2d     center       = get_center();

    return { center.x + std::cos(angle) * spawn_radius,
             center.y + std::sin(angle) * spawn_radius };
}
float landscape::get_angle_to(const transform2d& pos) const
{
    Kengine::transform2d center_pos = get_center();
    Kengine::transform2d vec        = pos - center_pos;
    return std::atan2(vec.y, vec.x);
}
float landscape::get_distance_to(const transform2d& pos) const
{
    Kengine::transform2d center_pos = get_center();
    return center_pos.distance(pos);
}
b2Body* landscape::get_body() const
{
    return l_body;
}
