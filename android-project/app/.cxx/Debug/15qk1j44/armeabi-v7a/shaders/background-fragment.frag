#version 300 es
precision mediump float;

in vec2 t_position;

out vec4 f_color;

uniform sampler2D space_texture;

void main() {
    f_color = texture2D(space_texture, t_position) * 0.7;
}
