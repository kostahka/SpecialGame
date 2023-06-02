#include "sprite.hxx"
#include "engine.hxx"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/trigonometric.hpp"
#include "shader-program.hxx"
#include "transform3d.hxx"
#include "vertex-array-object.hxx"
#include <numbers>

namespace Kengine
{

sprite::sprite(texture_object*    texture,
               const irect&       uv,
               const transform2d& pos,
               const transform2d& size,
               game*              game)
    : texture(texture)
    , uv(uv)
    , pos(pos)
    , size(size)
    , sprite_game(game)
    , origin({ 0, 0 })
{
    vertices.resize(6);

    itransform2d tex_size = texture->get_size();

    const transform3d v1 = { 0, 0, 0 };
    const transform3d v2 = { 0, 1, 0 };
    const transform3d v3 = { 1, 1, 0 };
    const transform3d v4 = { 1, 0, 0 };

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

    vao     = create_vao(vertices);
    program = sprite_program;
};

void sprite::draw()
{
    texture->bind();
    program->use();
    program->set_uniform_matrix4fv("projection", sprite_game->projection);
    program->set_uniform_matrix4fv("view", sprite_game->view);
    glm::mat4 model(1);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, 0));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, glm::vec3(size.x, size.y, 0));
    model = glm::translate(model, glm::vec3(-origin.x, -origin.y, 0));
    program->set_uniform_matrix4fv("model", model);
    vao->draw_triangles(6);
};

void sprite::set_origin(const transform2d& origin)
{
    this->origin = origin;
};

void sprite::set_uv(const irect& uv)
{
    this->uv = uv;
};
void sprite::set_texture(texture_object* texture)
{
    this->texture = texture;
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
};
} // namespace Kengine
