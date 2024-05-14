#version 300 es
precision mediump float;

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec2 in_t_position;

out vec2 t_position;

layout(std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    t_position = in_t_position;
    vec4 fix_position =
        projection * view *
        vec4(v_position.x * 10.f, v_position.y * 10.f, -89.f, 1.0);
    gl_Position = vec4(fix_position.xy * 0.01, fix_position.z, 1.0);
}
