#include "scene/planet-scene.hxx"

#include "game.hxx"
#include "physics/physics.hxx"
#include "render/game-gui.hxx"
#include "resources.hxx"

const std::string planet_scene::name = "planet_scene";

planet_scene::planet_scene()
    : game_player(nullptr)
{
    state_id[planet_start_state::name] = 0;
    states.push_back(new planet_start_state(this));
    state_id[planet_play_state::name] = 1;
    states.push_back(new planet_play_state(this));
    state_id[planet_end_state::name] = 2;
    states.push_back(new planet_end_state(this));
    state_id[planet_pause_state::name] = 3;
    states.push_back(new planet_pause_state(this));
    current_state = 0;
}

planet_scene::~planet_scene()
{
    for (auto& state : states)
    {
        delete state;
    }
}

void planet_scene::on_start()
{
    physics::land.init();

    game_player = new player(physics::land.get_spawn_place(0));
    e_spawner.init(game_player->get_astronaut());
    current_game->game_cursor->set_cursor(cursor_type::attack);
}

const std::string planet_start_state::name = "planet_start";

planet_start_state::planet_start_state(planet_scene* scene)
    : state_scene(scene)
{
}

void planet_start_state::on_event(const Kengine::event::game_event& event)
{
    using namespace Kengine::event;
    if ((event.g_type == type::keyboard_event && event.keyboard.pressed) ||
        (event.g_type == type::touch_event && event.touch.pressed))
    {
        state_scene->set_state(planet_play_state::name);
    }
}

void planet_start_state::update(
    std::chrono::duration<int, std::milli> delta_time)
{
    physics::update(delta_time);

    state_scene->update_game_objects(delta_time);
    state_scene->control_game_objects(delta_time);
}

void planet_start_state::render(
    std::chrono::duration<int, std::milli> delta_time)
{
    state_scene->render_game_objects(delta_time);

    physics::land.draw();
}

void planet_start_state::imgui_render()
{
    gui::draw_training_hint();
}

const std::string planet_play_state::name = "planet_play";

planet_play_state::planet_play_state(planet_scene* scene)
    : state_scene(scene)
{
}

void planet_play_state::on_event(const Kengine::event::game_event& e)
{
    using namespace Kengine::event;
    using namespace Kengine::input;
#ifdef __ANDROID__
    current_game->aim_joystick->on_event(e);
    current_game->move_joystick->on_event(e);
#endif
    if (e.g_type == type::keyboard_event &&
        (e.keyboard.key == keyboard::key::key_escape ||
         e.keyboard.key == keyboard::key::key_ac_back) &&
        e.keyboard.pressed)
    {
        state_scene->set_state(planet_pause_state::name);
        current_game->game_cursor->set_cursor(cursor_type::simple);
    }
    state_scene->on_event_game_objects(e);
}

void planet_play_state::update(
    std::chrono::duration<int, std::milli> delta_time)
{
    physics::update(delta_time);
    state_scene->e_spawner.update(delta_time);

    state_scene->update_game_objects(delta_time);
    state_scene->control_game_objects(delta_time);
    if (state_scene->game_player->get_hp() <= 0)
    {
        state_scene->set_state(planet_end_state::name);
        current_game->game_cursor->set_cursor(cursor_type::simple);
    }
}

void planet_play_state::render(
    std::chrono::duration<int, std::milli> delta_time)
{

    state_scene->render_game_objects(delta_time);

    physics::land.draw();
#ifdef __ANDROID__
    current_game->aim_joystick->draw();
    current_game->move_joystick->draw();
#endif
}

void planet_play_state::imgui_render()
{
    state_scene->e_spawner.imgui_render();

    int reselect_gun =
        gui::draw_selected_gun(state_scene->game_player->get_selected_gun());
    state_scene->game_player->select_gun(reselect_gun);
    gui::draw_health_bar(state_scene->game_player->get_hp());
    gui::draw_score(state_scene->e_spawner.get_killed_enemies());
}

const std::string planet_end_state::name = "planet_end";

planet_end_state::planet_end_state(planet_scene* scene)
    : state_scene(scene)
{
}

void planet_end_state::on_event(const Kengine::event::game_event&) {}

void planet_end_state::update(std::chrono::duration<int, std::milli> delta_time)
{
}

void planet_end_state::render(std::chrono::duration<int, std::milli> delta_time)
{
    state_scene->render_game_objects(delta_time);

    physics::land.draw();
}

void planet_end_state::imgui_render()
{
    if (gui::draw_end_menu(state_scene->e_spawner.get_killed_enemies()))
    {
        state_scene->destroy_all_game_objects();
        state_scene->destroy_all_controllers();
        state_scene->on_start();
        state_scene->set_state(planet_start_state::name);
    }
}

const std::string planet_pause_state::name = "planet_pause";

planet_pause_state::planet_pause_state(planet_scene* scene)
    : state_scene(scene)
{
}

void planet_pause_state::on_event(const event::game_event& e)
{
    using namespace Kengine::event;
    using namespace Kengine::input;
    if (e.g_type == type::keyboard_event &&
        e.keyboard.key == input::keyboard::key::key_escape &&
        e.keyboard.pressed)
    {
        state_scene->set_state(planet_play_state::name);
        current_game->game_cursor->set_cursor(
            state_scene->game_player->get_selected_gun() == 2
                ? cursor_type::shovel
                : cursor_type::attack);
    }
}

void planet_pause_state::update(
    std::chrono::duration<int, std::milli> delta_time)
{
}

void planet_pause_state::render(
    std::chrono::duration<int, std::milli> delta_time)
{
    state_scene->render_game_objects(delta_time);

    physics::land.draw();
}

void planet_pause_state::imgui_render()
{
    int button = gui::draw_pause_menu();
    if (button == 1)
    {
        state_scene->set_state(planet_play_state::name);
        current_game->game_cursor->set_cursor(
            state_scene->game_player->get_selected_gun() == 2
                ? cursor_type::shovel
                : cursor_type::attack);
    }
    else if (button == 2)
    {
        state_scene->destroy_all_game_objects();
        state_scene->destroy_all_controllers();
        state_scene->on_start();
        state_scene->set_state(planet_start_state::name);
    }
}
