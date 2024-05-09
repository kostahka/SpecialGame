#version 300 es
precision mediump float;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_tex_coord;
out vec2 v_tex_coord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    v_tex_coord = a_tex_coord;
    gl_Position = projection * view * model * vec4(a_position, 1.0);
}
