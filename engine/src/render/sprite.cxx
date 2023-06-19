#include "Kengine/render/sprite.hxx"

#include "Kengine/engine.hxx"
#include "Kengine/render/engine-resources.hxx"
#include "Kengine/render/vertex-array-object.hxx"
#include "Kengine/transform3d.hxx"

#include "glm/ext/matrix_transform.hpp"

namespace Kengine
{

constexpr transform3d v1 = { 0, 0, 0 };
constexpr transform3d v2 = { 0, 1, 0 };
constexpr transform3d v3 = { 1, 1, 0 };
constexpr transform3d v4 = { 1, 0, 0 };

void world_sprite_program_use(shader_program* program)
{
    program->use();
    program->set_uniform_matrix4fv("projection",
                                   engine::instance()->e_game->projection);
    program->set_uniform_matrix4fv("view", engine::instance()->e_game->view);
}

void local_sprite_program_use(shader_program* program)
{
    program->use();
    program->set_uniform_matrix4fv("projection",
                                   engine::instance()->e_game->projection);
}

sprite::sprite(texture_object*    texture,
               const irect&       uv,
               const transform2d& pos,
               const transform2d& size,
               bool               world_sprite,
               float              z)
    : texture(texture)
    , uv(uv)
    , pos(pos)
    , size(size)
    , origin({ 0, 0 })
    , angle(0)
    , z(z)
{
    vertices.resize(6);

    itransform2d tex_size = texture->get_size();

    const transform2d t1 = { static_cast<float>(uv.x) / tex_size.x,
                             static_cast<float>(uv.y + uv.h) / tex_size.y };
    const transform2d t2 = { static_cast<float>(uv.x) / tex_size.x,
                             static_cast<float>(uv.y) / tex_size.y };
    const transform2d t3 = { static_cast<float>(uv.x + uv.w) / tex_size.x,
                             static_cast<float>(uv.y) / tex_size.y };
    const transform2d t4 = { static_cast<float>(uv.x + uv.w) / tex_size.x,
                             static_cast<float>(uv.y + uv.h) / tex_size.y };

    vertices[0] = { v1, t1 };
    vertices[1] = { v2, t2 };
    vertices[2] = { v3, t3 };

    vertices[3] = { v3, t3 };
    vertices[4] = { v4, t4 };
    vertices[5] = { v1, t1 };

    vao = create_vao(vertices);
    if (world_sprite)
    {
        use_func = world_sprite_program_use;
        program  = e_resources::global_sprite_program;
    }
    else
    {
        use_func = local_sprite_program_use;
        program  = e_resources::local_sprite_program;
    }
};

void sprite::draw() const
{
    texture->bind();
    use_func(program);
    glm::mat4 model(1);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, z));
    model = glm::rotate(model, angle, glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, glm::vec3(size.x, size.y, 1));
    model = glm::translate(model, glm::vec3(-origin.x, -origin.y, 0));
    program->set_uniform_matrix4fv("model", model);
    vao->draw_triangles(2);
};

void sprite::set_origin(const transform2d& origin)
{
    this->origin = origin;
};

void sprite::set_uv(const irect& uv)
{
    this->uv = uv;
    set_texture_coords();
};
void sprite::set_texture(texture_object* texture)
{
    this->texture = texture;
    set_texture_coords();
};
void sprite::set_pos(const transform2d& pos)
{
    this->pos = pos;
};
void sprite::set_angle(const float angle)
{
    this->angle = angle;
};
void sprite::set_size(const transform2d& size)
{
    this->size = size;
};

transform2d sprite::get_origin() const
{
    return origin;
};
irect sprite::get_uv() const
{
    return uv;
};
texture_object* sprite::get_texture() const
{
    return texture;
};
transform2d sprite::get_pos() const
{
    return pos;
};
float sprite::sprite::get_angle() const
{
    return angle;
};
transform2d sprite::get_size() const
{
    return size;
}
void sprite::set_texture_coords()
{
    itransform2d tex_size = texture->get_size();

    const transform2d t1 = { static_cast<float>(uv.x) / tex_size.x,
                             static_cast<float>(uv.y + uv.h) / tex_size.y };
    const transform2d t2 = { static_cast<float>(uv.x) / tex_size.x,
                             static_cast<float>(uv.y) / tex_size.y };
    const transform2d t3 = { static_cast<float>(uv.x + uv.w) / tex_size.x,
                             static_cast<float>(uv.y) / tex_size.y };
    const transform2d t4 = { static_cast<float>(uv.x + uv.w) / tex_size.x,
                             static_cast<float>(uv.y + uv.h) / tex_size.y };

    vertices[0] = { v1, t1 };
    vertices[1] = { v2, t2 };
    vertices[2] = { v3, t3 };

    vertices[3] = { v3, t3 };
    vertices[4] = { v4, t4 };
    vertices[5] = { v1, t1 };

    vao->set_vertices(
        vertices.data(), 0, vertices.size() * sizeof(vertex_text2d));
}
} // namespace Kengine
