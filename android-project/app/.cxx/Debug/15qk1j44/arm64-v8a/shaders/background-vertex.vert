#version 300 es
precision mediump float;

layout (location = 0) in vec3 v_position;

out vec2 t_position;

void main() {
    t_position = v_position.xy * 3.0;
    gl_Position = vec4(v_position, 1.0);
}
