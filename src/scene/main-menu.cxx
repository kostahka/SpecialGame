#include "scene/main-menu.hxx"
#include "game.hxx"
#include "render/game-gui.hxx"
#include "scene/planet-scene.hxx"

const std::string main_menu_scene::name = "main_menu_scene";

main_menu_scene::main_menu_scene()
{
    state_id[main_menu_state::name] = 0;
    states.push_back(new main_menu_state());
    current_state = 0;
}

void main_menu_scene::on_start() {}

main_menu_scene::~main_menu_scene() = default;

const std::string main_menu_state::name = "main_menu";

main_menu_state::main_menu_state() = default;

void main_menu_state::on_event(const Kengine::event::game_event&) {}

void main_menu_state::update(std::chrono::duration<int, std::milli> delta_time)
{
}

void main_menu_state::render(std::chrono::duration<int, std::milli> delta_time)
{
}

void main_menu_state::imgui_render()
{
    if (gui::draw_main_menu())
    {
        current_game->set_scene(planet_scene::name);
    }
}
