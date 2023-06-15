#include "game.hxx"

#include "Kengine/engine.hxx"
#include "Kengine/io/input.hxx"
#include "Kengine/render/texture.hxx"
#include "glm/ext/matrix_clip_space.hpp"
#include "imgui_internal.h"

#include "physics/physics.hxx"
#include "render/resources.hxx"
#include "scene/planet-scene.hxx"

using namespace Kengine;

static bool debug_draw = false;

my_game* current_game{ nullptr };

std::chrono::high_resolution_clock game_clock;

my_game::my_game(engine* game_engine)
    : game("My game", game_engine){};

void my_game::on_start()
{
    resources::init();

    start_point            = game_clock.now();
    float proj_width_half  = configuration.screen_width / 2.0f;
    float proj_height_half = configuration.screen_height / 2.0f;
    projection             = glm::ortho(-proj_width_half,
                            proj_width_half,
                            -proj_height_half,
                            proj_height_half,
                            -50.0f,
                            50.0f);
    view                   = glm::mat4(1);

    game_cursor = new cursor();

    game_cursor->set_cursor(cursor_type::simple);

    current_scene = new planet_scene();
    current_scene->on_start();

    physics::init();

    game_engine->set_cursor_visible(false);
};

void my_game::on_event(event::game_event e)
{
    current_scene->get_current_state()->on_event(e);
};

void my_game::on_update(std::chrono::duration<int, std::milli> delta_time)
{
    current_scene->get_current_state()->update(delta_time);
};

void my_game::on_render(std::chrono::duration<int, std::milli> delta_time)
{
    game_engine->clear_color({ 0.0, 0.0, 0.0, 0.0 });

    current_scene->get_current_state()->render(delta_time);

    if (debug_draw)
    {
        physics::physics_world.DebugDraw();
        physics::physics_draw.Draw();
    }

    game_engine->draw_imgui();

    game_cursor->draw();

    game_engine->swap_buffers();
};

void my_game::on_imgui_render()
{
    current_scene->get_current_state()->imgui_render();

    // clang-format off
    constexpr ImGuiWindowFlags game_gui_flags =
                                            0 |
                                            ImGuiWindowFlags_NoBackground |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoNav |
                                            ImGuiWindowFlags_NoBackground |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoScrollbar |
                                            ImGuiWindowFlags_NoScrollWithMouse |
                                            ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoTitleBar
;
    // clang-format on
    if (ImGui::Begin("Guns", 0, game_gui_flags))
    {
        ImGui::SetWindowPos({ 0, 0 });
        ImGui::Checkbox("Debug draw", &debug_draw);

        ImGui::End();
    }
    if (ImGui::Begin("Develop"))
    {
        ImGui::Checkbox("Debug draw", &debug_draw);
    }
    ImGui::End();

    // ImGui::ShowDemoWindow();
};

my_game::~my_game() {}

void my_game::add_game_object(game_object* obj)
{
    current_scene->add_game_object(obj);
}
void my_game::remove_game_object(game_object* obj)
{
    current_scene->remove_game_object(obj);
};

game* create_game(engine* e)
{
    if (e != nullptr)
    {
        if (!current_game)
            current_game = new my_game(e);
        return current_game;
    }

    return nullptr;
};
