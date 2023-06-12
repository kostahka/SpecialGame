#include "engine-resources.hxx"
namespace Kengine
{
namespace e_resources
{

void init()
{
    global_sprite_program = create_shader_program_from_code(R"(
                #version 300 es
                precision mediump float;

                layout (location = 0) in vec3 a_position;
                layout (location = 1) in vec2 a_tex_coord;
                out vec2 v_tex_coord;

                uniform mat4 projection;
                uniform mat4 view;
                uniform mat4 model;

                void main()
                {
                    v_tex_coord = a_tex_coord;
                    gl_Position = projection * view * model * vec4(a_position, 1.0);
                }
)",
                                                            R"(
                #version 300 es
                precision mediump float;

                in vec2 v_tex_coord;
                uniform sampler2D s_texture;

                out vec4 fragColor;

                void main()
                {
                    fragColor = texture2D(s_texture, v_tex_coord);
                }
)");
    local_sprite_program  = create_shader_program_from_code(R"(
                #version 300 es
                precision mediump float;

                layout (location = 0) in vec3 a_position;
                layout (location = 1) in vec2 a_tex_coord;
                out vec2 v_tex_coord;

                uniform mat4 projection;
                uniform mat4 model;

                void main()
                {
                    v_tex_coord = a_tex_coord;
                    gl_Position = projection * model * vec4(a_position, 1.0);
                }
)",
                                                           R"(
                #version 300 es
                precision mediump float;

                in vec2 v_tex_coord;
                uniform sampler2D s_texture;

                out vec4 fragColor;

                void main()
                {
                    fragColor = texture2D(s_texture, v_tex_coord);
                }
)");
    points_program        = create_shader_program_from_code(R"(
                #version 300 es
                precision mediump float;

                layout (location = 0) in vec3 v_position;
                layout (location = 1) in vec4 v_color;
                layout (location = 2) in float v_size;
                out vec4 f_color;

                uniform mat4 projection;
                uniform mat4 view;

                void main()
                {
                    f_color = v_color;
                    gl_Position = projection * view * vec4(v_position, 1.0);
                    gl_PointSize = v_size;
                }
)",
                                                     R"(
                #version 300 es
                precision mediump float;

                in vec4 f_color;

                out vec4 fragColor;

                void main()
                {
                    fragColor = f_color;
                }
)");
    lines_program         = create_shader_program_from_code(R"(
                #version 300 es
                precision mediump float;

                layout (location = 0) in vec3 v_position;
                layout (location = 1) in vec4 v_color;
                out vec4 f_color;

                uniform mat4 projection;
                uniform mat4 view;

                void main()
                {
                    f_color = v_color;
                    gl_Position = projection * view * vec4(v_position, 1.0);
                }
)",
                                                    R"(
                #version 300 es
                precision mediump float;

                in vec4 f_color;

                out vec4 fragColor;

                void main()
                {
                    fragColor = f_color;
                }
)");
};

shader_program* local_sprite_program{ nullptr };
shader_program* global_sprite_program{ nullptr };
shader_program* points_program{ nullptr };
shader_program* lines_program{ nullptr };
} // namespace e_resources
} // namespace Kengine