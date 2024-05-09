#version 300 es
precision mediump float;

layout (location = 0) in vec3 v_position;

out vec2 t_position;

uniform mat4 projection;
uniform mat4 view;

void main() {
    vec4 fix_position = projection * view * vec4(v_position.x * 60000.0f, v_position.y * 30000.0f, v_position.z, 1.0);
    t_position = fix_position.xy;
    gl_Position = vec4(v_position, 1.0);
}
