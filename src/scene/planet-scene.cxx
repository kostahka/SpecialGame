#include "scene/planet-scene.hxx"
#include "physics/physics.hxx"
#include "render/camera.hxx"

planet_scene::planet_scene()
    : current_state(-1)
{
    state_id[planet_start_state::name] = 0;
    states.push_back(new planet_start_state(this));
    state_id[planet_play_state::name] = 1;
    states.push_back(new planet_play_state(this));
    state_id[planet_end_state::name] = 2;
    states.push_back(new planet_end_state(this));
    current_state = 1;
}

scene_state* planet_scene::get_current_state()
{
    if (current_state < 0)
        return nullptr;

    return states[current_state];
}
void planet_scene::set_state(const std::string& name)
{
    if (state_id.contains(name))
        current_state = state_id[name];
}
planet_scene::~planet_scene()
{
    for (int i = 0; i < states.size(); i++)
    {
        delete states[i];
    }
}
void planet_scene::add_game_object(game_object* obj)
{
    game_objects.push_back(obj);
}
void planet_scene::remove_game_object(game_object* obj)
{
    game_objects.erase(
        std::remove(game_objects.begin(), game_objects.end(), obj));
}

void planet_scene::on_start()
{
    game_player = new player({ 500, 500 });
    physics::land.init();
}

const std::string planet_start_state::name = "planet_start";

planet_start_state::planet_start_state(planet_scene* scene)
    : state_scene(scene)
{
}

void planet_start_state::on_event(const Kengine::event::game_event&) {}

void planet_start_state::update(
    std::chrono::duration<int, std::milli> delta_time)
{
}

void planet_start_state::render(
    std::chrono::duration<int, std::milli> delta_time)
{
}

void planet_start_state::imgui_render() {}

const std::string planet_play_state::name = "planet_play";

planet_play_state::planet_play_state(planet_scene* scene)
    : state_scene(scene)
{
}

void planet_play_state::on_event(const Kengine::event::game_event& e)
{
    state_scene->game_player->on_event(e);
}

void planet_play_state::update(
    std::chrono::duration<int, std::milli> delta_time)
{
    physics::update(delta_time);

    state_scene->game_player->input_process();

    for (game_object* obj : state_scene->game_objects)
    {
        obj->update(delta_time);
    }

    Kengine::transform2d player_pos = state_scene->game_player->get_pos();
    camera::look_at(player_pos, player_pos - physics::land.get_center());
}

void planet_play_state::render(
    std::chrono::duration<int, std::milli> delta_time)
{
    for (game_object* obj : state_scene->game_objects)
    {
        obj->render(delta_time);
    }
    physics::land.draw();
}

void planet_play_state::imgui_render() {}

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
}

void planet_end_state::imgui_render() {}