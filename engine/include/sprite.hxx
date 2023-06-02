#pragma once

#include "engine.hxx"
#include "shader-program.hxx"
#include "texture.hxx"
#include "transform3d.hxx"
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

class sprite
{
public:
    sprite(texture_object*,
           const irect&       uv,
           const transform2d& pos,
           const transform2d& size,
           game*              s_game);
    void draw();

    void set_origin(const transform2d&);
    void set_uv(const irect&);
    void set_texture(texture_object*);
    void set_pos(const transform2d&);
    void set_angle(const float);
    void set_size(const transform2d&);

    transform2d     get_origin() const;
    irect           get_uv() const;
    texture_object* get_texture() const;
    transform2d     get_pos() const;
    float           get_angle() const;
    transform2d     get_size() const;

private:
    texture_object* texture;
    irect           uv;
    transform2d     origin;
    transform2d     pos;
    transform2d     size;
    float           angle;
    game*           sprite_game;

    vertex_array_object* vao;
    vertex_text2d_array  vertices;
    shader_program*      program;
};
} // namespace Kengine
