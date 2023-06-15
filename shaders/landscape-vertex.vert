#version 300 es
precision mediump float;

layout (location = 0) in vec2 v_position;

out vec2 t_position;

uniform mat4 projection;
uniform mat4 view;
uniform float cell_size;

void main() {
    t_position = v_position / cell_size / 20.0;
    gl_Position = projection * view * vec4(v_position, 0.0, 1.0);
}
