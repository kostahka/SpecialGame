#include "Kengine/render/draw-primitives.hxx"

#include "../../../include/game.hxx"
#include "Kengine/render/engine-resources.hxx"
#include "Kengine/render/shader-program.hxx"
#include "Kengine/render/vertex-array-object.hxx"

namespace Kengine
{
constexpr size_t max_vertices = 3 * 512;

struct gl_render_points : public gl_render_primitive
{
    void create() override
    {
        program = e_resources::points_program;
        vao     = create_vao(vertices, max_vertices);
        m_count = 0;
    };
    void destroy() override
    {
        delete vao;
        program = nullptr;
    };
    void vertex(const transform3d& pos,
                const color&       col,
                const float        size) override
    {
        if (m_count == max_vertices)
            draw();

        vertices[m_count] = { pos, col, size };
        ++m_count;
    };
    void draw() override
    {
        if (m_count == 0)
            return;

        vao->set_vertices(vertices, 0, m_count * sizeof(vertex_color_size));

        program->use();
        game* current_game = engine::instance()->e_game;

        program->set_uniform_matrix4fv("projection", current_game->projection);
        program->set_uniform_matrix4fv("view", current_game->view);
        vao->draw_points(m_count);
        m_count = 0;
    };

    shader_program*      program;
    vertex_array_object* vao;
    vertex_color_size    vertices[max_vertices];
    size_t               m_count;
};

struct gl_render_lines : public gl_render_primitive
{
    void create() override
    {
        program = e_resources::lines_program;
        vao     = create_vao(vertices, max_vertices);
        m_count = 0;
    };
    void destroy() override
    {
        delete vao;
        program = nullptr;
    };
    void vertex(const transform3d& pos,
                const color&       col,
                const float        size) override
    {
        if (m_count == max_vertices)
            draw();

        vertices[m_count] = { pos, col };
        ++m_count;
    };
    void draw() override
    {
        if (m_count == 0)
            return;

        vao->set_vertices(vertices, 0, m_count * sizeof(vertex_color));

        program->use();
        game* current_game = engine::instance()->e_game;

        program->set_uniform_matrix4fv("projection", current_game->projection);
        program->set_uniform_matrix4fv("view", current_game->view);
        vao->draw_lines(m_count);
        m_count = 0;
    };

    shader_program*      program;
    vertex_array_object* vao;
    vertex_color         vertices[max_vertices];
    size_t               m_count;
};

struct gl_render_triangles : public gl_render_primitive
{
    void create() override
    {
        program = e_resources::lines_program;
        vao     = create_vao(vertices, max_vertices);
        m_count = 0;
    };
    void destroy() override
    {
        delete vao;
        program = nullptr;
    };
    void vertex(const transform3d& pos,
                const color&       col,
                const float        size) override
    {
        if (m_count == max_vertices)
            draw();

        vertices[m_count] = { pos, col };
        ++m_count;
    };
    void draw() override
    {
        if (m_count == 0)
            return;

        vao->set_vertices(vertices, 0, m_count * sizeof(vertex_color));

        program->use();
        game* current_game = engine::instance()->e_game;

        program->set_uniform_matrix4fv("projection", current_game->projection);
        program->set_uniform_matrix4fv("view", current_game->view);
        vao->draw_triangles(m_count / 3);
        m_count = 0;
    };

    shader_program*      program;
    vertex_array_object* vao;
    vertex_color         vertices[max_vertices];
    size_t               m_count;
};

gl_render_primitive* create_primitive_render(primitive_type type)
{
    switch (type)
    {

        case primitive_type::points:
            return new gl_render_points();
            break;
        case primitive_type::lines:
            return new gl_render_lines();
            break;
        case primitive_type::triangles:
            return new gl_render_triangles();
            break;
    }
};

gl_render_primitive::~gl_render_primitive() = default;
} // namespace Kengine