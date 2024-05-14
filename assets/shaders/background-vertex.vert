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
    vec4 fix_position =
        inverse(view) * inverse(projection) *
        vec4(v_position.x * 10000.f, v_position.y * 10000.f, 0.f, 1.0);
    t_position  = fix_position.xy * 0.00001;
    gl_Position = vec4(v_position, 1.f, 1.0);
}
