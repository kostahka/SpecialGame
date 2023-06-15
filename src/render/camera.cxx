#include "render/camera.hxx"
#include <numbers>

#include "game.hxx"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"

namespace camera
{
static glm::vec3 camera_pos(0.f, 0.f, 10.f);
static glm::vec3 camera_target(0.f, 0.f, 0.f);
static glm::vec3 camera_up(0.f, 1.f, 0.f);
float            camera_angle = 0;

void look_at(const Kengine::transform2d& pos, float angle)
{
    camera_angle       = angle;
    camera_pos         = glm::vec3(pos.x, pos.y, 10.f);
    camera_target      = glm::vec3(pos.x, pos.y, 0.f);
    camera_up          = glm::vec3(std::cos(angle), std::sin(angle), 0.f);
    current_game->view = glm::lookAt(camera_pos, camera_target, camera_up);
};
void look_at(const Kengine::transform2d& pos,
             const Kengine::transform2d& up_vector)
{
    camera_pos    = glm::vec3(pos.x, pos.y, 10.f);
    camera_target = glm::vec3(pos.x, pos.y, 0.f);
    camera_up     = glm::vec3(up_vector.x, up_vector.y, 0.f);
    camera_angle  = std::atan2f(up_vector.y, up_vector.x) / std::numbers::pi;
    current_game->view = glm::lookAt(camera_pos, camera_target, camera_up);
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
    glm::vec4 pos(t.x - current_game->configuration.screen_width / 2,
                  current_game->configuration.screen_height / 2 - t.y,
                  0,
                  1);
    glm::vec4 global_pos = glm::inverse(current_game->view) * pos;
    return { global_pos.x, global_pos.y };
};

Kengine::transform2d screen_to_local(const transform2d& t)
{
    glm::vec4 pos(t.x - current_game->configuration.screen_width / 2.0f,
                  current_game->configuration.screen_height / 2.0f - t.y,
                  0,
                  1);
    return { pos.x, pos.y };
};

} // namespace camera