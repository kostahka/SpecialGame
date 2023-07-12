#include "render/camera.hxx"

#include "game.hxx"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "render/background.hxx"
#include "render/game-gui.hxx"

namespace camera
{
static glm::vec3 camera_pos(0.f, 0.f, 10.f);
static glm::vec3 camera_target(0.f, 0.f, 0.f);
static glm::vec3 camera_up(0.f, 1.f, 0.f);
#ifdef __ANDROID__
static float scale = 8.f;
#else
static float scale = 5.f;
#endif
float camera_angle = 0;

float proj_width_half;
float proj_height_half;

constexpr float gui_scale_m         = 1300;
constexpr float android_draw_height = 300;

void init()
{
#ifdef __ANDROID__
    float screen_k =
        static_cast<float>(current_game->configuration.screen_width) /
        static_cast<float>(current_game->configuration.screen_height);

    proj_width_half  = screen_k * android_draw_height;
    proj_height_half = android_draw_height;
#else
    proj_width_half =
        static_cast<float>(current_game->configuration.screen_width) / 2.0f;
    proj_height_half =
        static_cast<float>(current_game->configuration.screen_height) / 2.0f;
#endif

    proj_width_half /= scale;
    proj_height_half /= scale;

#ifdef __ANDROID__
    float joysticks_size =
        (static_cast<float>(current_game->configuration.screen_width) +
         static_cast<float>(current_game->configuration.screen_height)) /
        80;
    current_game->aim_joystick->set_pos(
        { proj_width_half - joysticks_size / 2.f,
          -proj_height_half + joysticks_size / 2.f });
    current_game->move_joystick->set_pos(
        { -proj_width_half + joysticks_size / 2.f,
          -proj_height_half + joysticks_size / 2.f });

    current_game->aim_joystick->set_size(joysticks_size);
    current_game->move_joystick->set_size(joysticks_size);
#endif

    current_game->projection = glm::ortho(-proj_width_half,
                                          proj_width_half,
                                          -proj_height_half,
                                          proj_height_half,
                                          -50.0f,
                                          50.0f);
    gui::gui_scale =
        (static_cast<float>(current_game->configuration.screen_width) +
         static_cast<float>(current_game->configuration.screen_height)) /
        gui_scale_m;

    float background_scale =
        static_cast<float>(current_game->configuration.screen_width) /
        static_cast<float>(current_game->configuration.screen_height);
    background::projection =
        glm::ortho(-proj_width_half - 5000 * background_scale,
                   proj_width_half + 5000 * background_scale,
                   -proj_height_half - 5000,
                   proj_height_half + 5000,
                   -50.0f,
                   50.0f);
}

void window_resize()
{

    proj_width_half =
        static_cast<float>(current_game->configuration.screen_width) / 2.0f;
    proj_height_half =
        static_cast<float>(current_game->configuration.screen_height) / 2.0f;
    proj_width_half /= scale;
    proj_height_half /= scale;
    current_game->projection = glm::ortho(-proj_width_half,
                                          proj_width_half,
                                          -proj_height_half,
                                          proj_height_half,
                                          -50.0f,
                                          50.0f);
    gui::gui_scale =
        (static_cast<float>(current_game->configuration.screen_width) +
         static_cast<float>(current_game->configuration.screen_height)) /
        gui_scale_m;

    float background_scale =
        static_cast<float>(current_game->configuration.screen_width) /
        static_cast<float>(current_game->configuration.screen_height);
    background::projection =
        glm::ortho(-proj_width_half - 5000 * background_scale,
                   proj_width_half + 5000 * background_scale,
                   -proj_height_half - 5000,
                   proj_height_half + 5000,
                   -50.0f,
                   50.0f);
}

void look_at(const Kengine::transform2d& pos,
             const Kengine::transform2d& up_vector)
{
    camera_pos    = glm::vec3(pos.x, pos.y, 10.f);
    camera_target = glm::vec3(pos.x, pos.y, 0.f);
    camera_up     = glm::vec3(up_vector.x, up_vector.y, 0.f);
    camera_angle  = std::atan2(up_vector.y, up_vector.x) / 3.14f;
    glm::mat4 view(1);

    view               = glm::lookAt(camera_pos, camera_target, camera_up);
    current_game->view = view;
};

float get_angle()
{
    return camera_angle;
};

Kengine::transform2d get_pos()
{
    return { camera_pos.x, camera_pos.y };
};

Kengine::transform2d screen_to_global(const transform2d& t)
{
    float pos_norm_x =
        t.x /
        (static_cast<float>(current_game->configuration.screen_width) / 2.f);
    float pos_norm_y =
        t.y /
        (static_cast<float>(current_game->configuration.screen_height) / 2.f);
    float     pos_x = (pos_norm_x - 1) * proj_width_half;
    float     pos_y = (1 - pos_norm_y) * proj_height_half;
    glm::vec4 pos(pos_x, pos_y, 0, 1);
    glm::vec4 global_pos = glm::inverse(current_game->view) * pos;
    return { global_pos.x, global_pos.y };
};

Kengine::transform2d screen_to_local(const transform2d& t)
{
    float pos_norm_x =
        t.x /
        (static_cast<float>(current_game->configuration.screen_width) / 2.f);
    float pos_norm_y =
        t.y /
        (static_cast<float>(current_game->configuration.screen_height) / 2.f);
    float     pos_x = (pos_norm_x - 1) * proj_width_half;
    float     pos_y = (1 - pos_norm_y) * proj_height_half;
    glm::vec4 pos(pos_x, pos_y, 0, 1);
    return { pos.x, pos.y };
};

} // namespace camera
