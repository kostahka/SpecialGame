#version 300 es
precision mediump float;

in vec2 out_tex_coord;

uniform float     time;
uniform sampler2D s_texture;

out vec4 fragColor;

void main()
{
    // float x_k = cos(out_tex_coord.x * time / 200.f) / 40.f;
    // float y_k = sin(out_tex_coord.y * time / 200.f) / 40.f;
    // fragColor = texture(s_texture, vec2(out_tex_coord.x + x_k * y_k,
    // out_tex_coord.y));
    float y_k = sin((out_tex_coord.y - 0.25) * 3.2f);
    float x_k = 0.5 * cos((out_tex_coord.x - 0.4f) * 500.f - time / 30.f) +
                0.5 * sin((out_tex_coord.y - 0.25) * 30.f);
    fragColor =
        texture(s_texture, vec2(out_tex_coord.x, 0.25 - 0.4 * x_k * y_k));
    fragColor = vec4(fragColor.r,
                     fragColor.g,
                     fragColor.b,
                     fragColor.a - (out_tex_coord.x - 0.4) / 0.1);
}
