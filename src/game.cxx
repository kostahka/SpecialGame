#include "game.hxx"

#include "components/astronaut-component.hxx"
#include "components/bullet-component.hxx"
#include "components/enemies-progress-bar-component.hxx"
#include "components/enemy-component.hxx"
#include "components/enemy-wave-text-component.hxx"
#include "components/game-over-menu-component.hxx"
#include "components/game-tutorial-menu-component.hxx"
#include "components/gui-button-component.hxx"
#include "components/gui-selectable-component.hxx"
#include "components/pause-menu-component.hxx"
#include "components/player-controller.hxx"
#include "components/player-gun-selectable-component.hxx"
#include "components/player-health-bar-component.hxx"
#include "components/progress-bar-component.hxx"
#include "resources/resources.hxx"
#include "system/astronaut-system.hxx"
#include "system/background-system.hxx"
#include "system/game-gui-system.hxx"
#include "system/landscape-system.hxx"
#include "system/planet-gameplay-system.hxx"
#include "system/player-system.hxx"

#include "Kengine/engine.hxx"
#include "Kengine/scene/scene-manager.hxx"
#include "Kengine/scene/scene.hxx"
#include "glm/ext/matrix_clip_space.hpp"

#include <iostream>

#ifdef __ANDROID__
 #define main Game_main
#endif

using namespace Kengine;

special_game* special_game::instance{ nullptr };

Kengine::string_id restart_game_event_id{};
Kengine::string_id start_game_event_id{};
Kengine::string_id exit_game_event_id{};

special_game::special_game()
    : Kengine::game()
{
    system_container::register_system<game_gui_system>(game_gui_system::name);
    system_container::register_system<landscape_system>(landscape_system::name);
    system_container::register_system<player_system>(player_system::name);
    system_container::register_system<astronaut_system>(astronaut_system::name);
    system_container::register_system<planet_gameplay_system>(
        planet_gameplay_system::name);
    system_container::register_system<background_system>(
        background_system::name);

    component_container::register_component<gui_button_component>(
        gui_button_component::name);
    component_container::register_component<game_over_menu_component>(
        game_over_menu_component::name);
    component_container::register_component<game_tutorial_menu_component>(
        game_tutorial_menu_component::name);
    component_container::register_component<pause_menu_component>(
        pause_menu_component::name);
    component_container::register_component<enemy_wave_text_component>(
        enemy_wave_text_component::name);
    component_container::register_component<enemies_progress_bar_component>(
        enemies_progress_bar_component::name);
    component_container::register_component<player_gun_selectable_component>(
        player_gun_selectable_component::name);
    component_container::register_component<gui_selectable_component>(
        gui_selectable_component::name);
    component_container::register_component<player_health_bar_component>(
        player_health_bar_component::name);
    component_container::register_component<progress_bar_component>(
        progress_bar_component::name);
    component_container::register_component<enemy_component>(
        enemy_component::name);
    component_container::register_component<player_controller>(
        player_controller::name);
    component_container::register_component<bullet_component>(
        bullet_component::name);
    component_container::register_component(
        Kengine::hash_string(astronaut_component::name),
        astronaut_component::info);

    instance = this;

    restart_game_event_id = Kengine::hash_string("restart-game");
    start_game_event_id   = Kengine::hash_string("start-game");
    exit_game_event_id    = Kengine::hash_string("exit-game");
}

void special_game::on_start()
{
    resources::initialize();
    set_current_scene(std::string_view("main-menu"));
}

void special_game::on_event(const Kengine::event::game_event& g_event)
{
    if (g_event.g_type == Kengine::event::type::gui_event)
    {
        if (g_event.gui.gui_id == restart_game_event_id ||
            g_event.gui.gui_id == start_game_event_id)
        {
            if (g_event.gui.mouse.pressed)
            {
                set_current_scene(std::string_view("planet"));
            }
        }
        else if (g_event.gui.gui_id == exit_game_event_id)
        {
            if (g_event.gui.mouse.pressed)
            {
                Kengine::quit();
            }
        }
    }
}

void special_game::on_update(int delta_ms) {}

void special_game::on_render(int delta_ms) {}

special_game::~special_game() = default;

Kengine::game* create_game()
{
    return new special_game();
}

#ifndef ENGINE_DEV
extern "C" int main(int argc, char* argv[])
{
    using namespace Kengine;
    Kengine::scene_manager::set_assets_base_folder(
        std::filesystem::current_path() / "assets");

    if (Kengine::run(&create_game, "Special game"))
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
};
#endif
