#pragma once

#include "Kengine/engine.hxx"
#include "Kengine/transform3d.hxx"
#include "shader-program.hxx"
#include "texture.hxx"
#include "vertex-array-object.hxx"
#include "vertex.hxx"
#include <vector>

namespace Kengine
{

struct irect
{
    int x;
    int y;
    int w;
    int h;
};

typedef void (*shader_program_use_func)(const glm::mat4& model);

class sprite
{
public:
    sprite(texture_object*,
           const irect&       uv,
           const transform2d& pos,
           const transform2d& size,
           bool               world_sprite = true,
           float              z            = 0);
    void draw() const;

    void set_origin(const transform2d&);
    void set_uv(const irect&);
    void set_texture(texture_object*);
    void set_pos(const transform2d&);
    void set_angle(const float);
    void set_size(const transform2d&);

    [[nodiscard]] transform2d     get_origin() const;
    [[nodiscard]] irect           get_uv() const;
    [[nodiscard]] texture_object* get_texture() const;
    [[nodiscard]] transform2d     get_pos() const;
    [[nodiscard]] float           get_angle() const;
    [[nodiscard]] transform2d     get_size() const;

    void set_program_use_func(shader_program_use_func use_func);

private:
    void set_texture_coords();

    texture_object* texture;
    irect           uv;
    transform2d     origin;
    transform2d     pos;
    transform2d     size;
    float           angle;
    float           z;

    shader_program_use_func use_func;
    vertex_array_object*    vao;
    vertex_text2d_array     vertices;
};
} // namespace Kengine
