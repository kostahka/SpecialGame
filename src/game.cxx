#include "game.hxx"

#include "Kengine/engine.hxx"
#include "Kengine/render/texture.hxx"
#include "glm/ext/matrix_clip_space.hpp"

#include "physics/physics.hxx"
#include "render/background.hxx"
#include "render/game-gui.hxx"
#include "render/resources.hxx"
#include "scene/main-menu.hxx"
#include "scene/planet-scene.hxx"

using namespace Kengine;

bool debug_draw = false;

my_game* current_game{ nullptr };

std::chrono::high_resolution_clock game_clock;

my_game::my_game(engine* game_engine)
    : game("My game", game_engine)
    , game_cursor(nullptr)
    , current_scene(nullptr)
{
    configuration.screen_width  = 1000;
    configuration.screen_height = 600;
}

void my_game::on_start()
{
    resources::init();

    start_point = game_clock.now();
    float proj_width_half =
        static_cast<float>(configuration.screen_width) / 2.0f;
    float proj_height_half =
        static_cast<float>(configuration.screen_height) / 2.0f;
    projection = glm::ortho(-proj_width_half,
                            proj_width_half,
                            -proj_height_half,
                            proj_height_half,
                            -50.0f,
                            50.0f);
    view       = glm::mat4(1);

    game_cursor = new cursor();

    game_cursor->set_cursor(cursor_type::simple);
    physics::init();

    background::init();

    scene_id[planet_scene::name] = 0;
    scenes.push_back(new planet_scene());
    scene_id[main_menu_scene::name] = 1;
    scenes.push_back(new main_menu_scene());

    set_scene(main_menu_scene::name);

    game_engine->set_cursor_visible(false);

    ImGui::GetStyle().WindowRounding = 10;
    ImGui::GetStyle().FrameRounding  = 10;

    resources::main_theme_sound->set_loop(true);
    resources::main_theme_sound->play();
}

void my_game::on_event(event::game_event e)
{
    current_scene->get_current_state()->on_event(e);
}

void my_game::on_update(std::chrono::duration<int, std::milli> delta_time)
{
    current_scene->get_current_state()->update(delta_time);
}

void my_game::on_render(std::chrono::duration<int, std::milli> delta_time)
{
    game_engine->clear_color({ 0.0, 0.0, 0.0, 0.0 });
    background::render();

    current_scene->get_current_state()->render(delta_time);

    if (debug_draw)
    {
        physics::physics_world.DebugDraw();
        physics::physics_draw.Draw();
    }

    game_engine->draw_imgui();

    game_cursor->draw();

    game_engine->swap_buffers();
}

void my_game::on_imgui_render()
{
    current_scene->get_current_state()->imgui_render();

    ImGui::SetNextWindowPos(
        { static_cast<float>(configuration.screen_width - 150), 0 });
    ImGui::SetNextWindowSize({ 150, 50 });

    if (ImGui::Begin("Develop", nullptr, gui::window_flags))
    {
        ImGui::Checkbox("Debug draw", &debug_draw);

        ImGui::End();
    }

    // ImGui::ShowDemoWindow();
}

my_game::~my_game() = default;

void my_game::add_game_object(game_object* obj)
{
    current_scene->add_game_object(obj);
}

void my_game::destroy_game_object(game_object* obj)
{
    current_scene->destroy_game_object(obj);
}

void my_game::add_controller(controller* c)
{
    current_scene->add_controller(c);
}

void my_game::destroy_controller(controller* c)
{
    current_scene->destroy_controller(c);
}

void my_game::set_scene(const std::string& name)
{
    if (scene_id.contains(name))
    {
        current_scene = scenes[scene_id[name]];
        current_scene->on_start();
    }
}

game* create_game(engine* e)
{
    if (e != nullptr)
    {
        if (!current_game)
            current_game = new my_game(e);
        return current_game;
    }

    return nullptr;
}
