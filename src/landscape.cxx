#include "landscape.hxx"
#include "PerlinNoise.hxx"
#include "engine.hxx"
#include "game.hxx"
#include "shader-program.hxx"
#include "texture.hxx"
#include "transform3d.hxx"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

landscape::landscape(game* l_game)
    : land_game(l_game){};

float interpolate_ground(float g1, float g2)
{
    // return (g1 - ground_value) / (g1 - g2);
    if (g1 > g2)
        return g1 + g2 - ground_value;
    else
        return 1.0 - g1 - g2 + ground_value;
}

void landscape::init()
{
    const siv::PerlinNoise::seed_type seed = time(NULL);
    const siv::PerlinNoise            perlin{ seed };
    const float                       map_fill = 0.55;

    const float  planet_radius = ground_w_count * 0.3;
    const size_t planet_x      = ground_w_count / 2;
    const size_t planet_y      = ground_h_count / 2;

    for (size_t y = 0; y < ground_h_count; y++)
    {
        for (size_t x = 0; x < ground_w_count; x++)
        {
            float delta = std::sqrt((planet_x - x) * (planet_x - x) +
                                    (planet_y - y) * (planet_y - y));
            if (delta < planet_radius)
            {
                float ground =
                    perlin.normalizedOctave2D_01(x / 8.0, y / 8.0, 2, 0.3);
                ground              = ground < ground_value
                                          ? ground / ground_value * map_fill
                                          : (ground / ground_value - 1) * (1 - map_fill) +
                                   ground_value;
                g_table[y][x].value = ground;
            }
            else
            {
                g_table[y][x].value = 0.1;
            }
        }
    }

    for (size_t i = ground_vertices_index; i < ground_horizontal_vertices_index;
         i++)
    {
        size_t x      = (i - ground_vertices_index) % ground_w_count;
        size_t y      = (i - ground_vertices_index) / ground_w_count;
        l_vertices[i] = { x * cell_size, y * cell_size };
    }
    calculate_vertices();
    calculate_indexes();
    vao            = Kengine::create_vao(l_vertices.data(),
                              l_vertices.size(),
                              l_indexes.data(),
                              l_indexes.size());
    program        = create_shader_program("./shaders/landscape-vertex.vert",
                                    "./shaders/landscape-fragment.frag");
    ground_texture = create_texture("./assets/ground.png");
};

landscape::~landscape()
{
    delete program;
};

void landscape::draw() const
{
    ground_texture->bind();
    program->use();
    program->set_uniform1f("cell_size", cell_size);
    program->set_uniform_matrix4fv("projection", land_game->projection);
    program->set_uniform_matrix4fv("view", land_game->view);
    vao->draw_triangles_elements(l_indexes.size());
};

void landscape::change_ground(float x, float y, float radius, float delta_value)
{
    size_t y_start = std::round((y - radius) / cell_size);
    size_t y_end   = std::round((y + radius) / cell_size);

    if (y_start < 0)
        y_start = 0;
    if (y_end >= ground_h_count)
        y_end = ground_h_count - 1;

    for (size_t y = y_start; y <= y_end; y++)
    {
        float r = std::sqrt(y * y + radius * radius);

        size_t x_start = std::round((x - r) / cell_size);
        size_t x_end   = std::round((x + r) / cell_size);

        if (x_start < 0)
            x_start = 0;
        if (x_end >= ground_w_count)
            x_end = ground_w_count - 1;

        for (size_t x = x_start; x <= x_end; x++)
        {
            g_table[y][x].value += delta_value;
            if (g_table[y][x].value > 1.0)
                g_table[y][x].value = 1.0;
            if (g_table[y][x].value < 0.0)
                g_table[y][x].value = 0.0;
            recalculate_vertices(x, y);
            recalculate_indexes(x, y);
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

        // getting vertice in horizontal line.
        float t =
            interpolate_ground(g_table[y][x].value, g_table[y][x + 1].value);
        l_vertices[i] = { (x + t) * cell_size, y * cell_size };
    }
    for (size_t i = ground_vertical_vertices_index;
         i < ground_vertices_end_index;
         i++)
    {
        // gettin x and y of vertices on vertical lines.
        size_t x = (i - ground_vertical_vertices_index) % ground_w_count;
        size_t y = (i - ground_vertical_vertices_index) / ground_w_count;

        // getting vertice in vertical line.
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
    const size_t cell_i = (y * (ground_w_count - 1) + x) * 4 * 3;

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

    switch (code)
    {
        case 0b0001:
            set_indexes(x, y, { v_i0, v_hi0, v_vi0 });
            break;
        case 0b0010:
            set_indexes(x, y, { v_vi1, v_i1, v_hi0 });
            break;
        case 0b0100:
            set_indexes(x, y, { v_i2, v_hi1, v_vi1 });
            break;
        case 0b1000:
            set_indexes(x, y, { v_i3, v_vi0, v_hi1 });
            break;
        case 0b0011:
            set_indexes(x, y, { v_i1, v_vi0, v_i0 }, { v_i1, v_vi1, v_vi0 });
            break;
        case 0b0110:
            set_indexes(x, y, { v_i2, v_hi0, v_i1 }, { v_i2, v_hi1, v_hi0 });
            break;
        case 0b1100:
            set_indexes(x, y, { v_i3, v_vi1, v_i2 }, { v_i3, v_vi0, v_vi1 });
            break;
        case 0b1001:
            set_indexes(x, y, { v_i0, v_hi1, v_i3 }, { v_i0, v_hi0, v_hi1 });
            break;
        case 0b1111:
            set_indexes(x, y, { v_i0, v_i1, v_i2 }, { v_i2, v_i3, v_i0 });
            break;
        case 0b0111:
            set_indexes(x,
                        y,
                        { v_i0, v_i1, v_vi0 },
                        { v_i1, v_hi1, v_vi0 },
                        { v_i1, v_i2, v_hi1 });
            break;
        case 0b1110:
            set_indexes(x,
                        y,
                        { v_i1, v_i2, v_hi0 },
                        { v_i2, v_vi0, v_hi0 },
                        { v_i2, v_i3, v_vi0 });
            break;
        case 0b1101:
            set_indexes(x,
                        y,
                        { v_i2, v_i3, v_vi1 },
                        { v_i3, v_hi0, v_vi1 },
                        { v_i3, v_i0, v_hi0 });
            break;
        case 0b1011:
            set_indexes(x,
                        y,
                        { v_i3, v_i0, v_hi1 },
                        { v_i0, v_vi1, v_hi1 },
                        { v_i0, v_i1, v_vi1 });
            break;
        case 0b0101:
            set_indexes(x,
                        y,
                        { v_i0, v_hi0, v_vi0 },
                        { v_i2, v_hi1, v_vi1 },
                        { v_hi0, v_hi1, v_vi0 },
                        { v_hi0, v_vi1, v_hi1 });
            break;
        case 0b1010:
            set_indexes(x,
                        y,
                        { v_i1, v_vi1, v_hi0 },
                        { v_i3, v_vi0, v_hi1 },
                        { v_hi0, v_vi1, v_vi0 },
                        { v_vi1, v_hi1, v_vi0 });
            break;
        default:
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

void landscape::recalculate_vertices(size_t x, size_t y)
{
    size_t left_i =
        ground_horizontal_vertices_index + y * (ground_w_count - 1) + x - 1;

    size_t right_i = left_i + 1;

    size_t down_i =
        ground_vertical_vertices_index + (y - 1) * (ground_w_count) + x;

    size_t up_i = ground_vertical_vertices_index + (y) * (ground_w_count) + x;

    if (x > 0)
    {
        float t =
            interpolate_ground(g_table[y][x - 1].value, g_table[y][x].value);
        l_vertices[left_i] = { (x - 1 + t) * cell_size, y * cell_size };
        set_vao_vertices(left_i);
    }
    if (x < ground_w_count - 1)
    {
        float t =
            interpolate_ground(g_table[y][x].value, g_table[y][x + 1].value);
        l_vertices[right_i] = { (x + t) * cell_size, y * cell_size };
        set_vao_vertices(right_i);
    }
    if (y > 0)
    {
        float t =
            interpolate_ground(g_table[y - 1][x].value, g_table[y][x].value);
        l_vertices[down_i] = { x * cell_size, (y - 1 + t) * cell_size };
        set_vao_vertices(down_i);
    }
    if (y < ground_h_count - 1)
    {
        float t =
            interpolate_ground(g_table[y][x].value, g_table[y + 1][x].value);
        l_vertices[up_i] = { x * cell_size, (y + t) * cell_size };
        set_vao_vertices(up_i);
    }
};

void landscape::recalculate_indexes(size_t x, size_t y)
{
    if (x > 0 && y > 0)
    {
        calculate_cell_indexes(x - 1, y - 1);
        set_vao_indexes(x - 1, y - 1);
    }
    if (x > 0 && y < ground_h_count - 1)
    {
        calculate_cell_indexes(x - 1, y);
        set_vao_indexes(x - 1, y);
    }
    if (y > 0 && x < ground_w_count - 1)
    {
        calculate_cell_indexes(x, y - 1);
        set_vao_indexes(x, y - 1);
    }
    if (x < ground_w_count - 1 && y < ground_h_count - 1)
    {
        calculate_cell_indexes(x, y);
        set_vao_indexes(x, y);
    }
};

void landscape::set_vao_indexes(size_t x, size_t y)
{
    const size_t cell_i = (y * (ground_w_count - 1) + x) * 4 * 3;

    uint32_t* data = &l_indexes[cell_i];
    long      offset =
        reinterpret_cast<char*>(data) - reinterpret_cast<char*>(&l_indexes[0]);
    long size = sizeof(uint32_t) * 4 * 3;
    vao->set_indexes(data, offset, size);
};

void landscape::set_vao_vertices(size_t index)
{
    transform2d* data = &l_vertices[index];
    long         offset =
        reinterpret_cast<char*>(data) - reinterpret_cast<char*>(&l_vertices[0]);
    long size = sizeof(transform2d);
    vao->set_vertices(data, offset, size);
};
