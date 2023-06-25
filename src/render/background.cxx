#include "render/background.hxx"

#include "Kengine/render/sprite.hxx"
#include "Kengine/render/texture.hxx"

#include "resources.hxx"

namespace background
{
static Kengine::texture_object*             space_texture;
static Kengine::static_vertex_array_object* space_vao;
static Kengine::shader_program*             space_progam;
static Kengine::sprite*                     planet_sprite;

void init()
{
    space_texture = Kengine::create_texture("assets/space.png");
    planet_sprite = new Kengine::sprite(resources::special_game_texture,
                                        { 2 * 64, 3 * 64, 128, 128 },
                                        { -30, 20 },
                                        { 30, 30 },
                                        false,
                                        -10);
    planet_sprite->set_origin({ 0.5, 0.5 });

    Kengine::vertex_array space_vertices;
    space_vertices.push_back({ -1, -1, 1 });
    space_vertices.push_back({ -1, 1, 1 });
    space_vertices.push_back({ 1, 1, 1 });
    space_vertices.push_back({ 1, 1, 1 });
    space_vertices.push_back({ 1, -1, 1 });
    space_vertices.push_back({ -1, -1, 1 });
    space_vao = Kengine::create_static_vao(space_vertices);

    space_progam = Kengine::create_shader_program(
        "shaders/background-vertex.vert", "shaders/background-fragment.frag");
}

void render()
{
    space_texture->bind();
    space_progam->use();
    space_vao->draw_triangles(2);

    planet_sprite->draw();
}
} // namespace background
