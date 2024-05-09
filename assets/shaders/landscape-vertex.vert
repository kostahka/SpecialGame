#version 300 es
precision mediump float;

layout(location = 0) in vec2 v_position;

out vec2 t_position;

layout(std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    t_position  = v_position / 40.0;
    gl_Position = projection * view * vec4(v_position, 0.0, 1.0);
}
