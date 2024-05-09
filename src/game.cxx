#include "game.hxx"

#include "components/astronaut-component.hxx"
#include "components/player-controller.hxx"
#include "resources/resources.hxx"
#include "system/astronaut-system.hxx"
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

special_game::special_game()
    : Kengine::game()
{
    system_container::register_system<landscape_system>(landscape_system::name);
    system_container::register_system<player_system>(player_system::name);
    system_container::register_system<astronaut_system>(astronaut_system::name);
    system_container::register_system<planet_gameplay_system>(
        planet_gameplay_system::name);

    component_container::register_component<player_controller>(
        player_controller::name);
    component_container::register_component(
        Kengine::hash_string(astronaut_component::name),
        astronaut_component::info);
}

void special_game::on_start()
{
    resources::initialize();
    set_current_scene(std::string_view("planet"));
}

void special_game::on_event(const Kengine::event::game_event& e)
{
    using namespace Kengine::event;
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
