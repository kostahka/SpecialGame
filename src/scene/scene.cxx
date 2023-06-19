#include "scene/scene.hxx"

scene::~scene() = default;
void scene::set_state(const std::string& name)
{
    if (state_id.contains(name))
        current_state = state_id[name];
}

void scene::add_game_object(game_object* obj)
{
    game_objects.push_back(obj);
}

void scene::destroy_game_object(game_object* obj)
{
    destroy_game_objects.push_back(obj);
}

void scene::add_controller(controller* c)
{
    controllers.push_back(c);
}

void scene::destroy_controller(controller* c)
{
    destroy_controllers.push_back(c);
}

void scene::update_game_objects(
    std::chrono::duration<int, std::milli> delta_time)
{
    for (game_object* obj : game_objects)
    {
        obj->update(delta_time);
    }
    for (auto& destroy_game_object : destroy_game_objects)
    {
        auto remove_go = std::remove(
            game_objects.begin(), game_objects.end(), destroy_game_object);
        if (remove_go != game_objects.end())
        {
            game_objects.erase(remove_go);
            delete destroy_game_object;
        }
    }
    destroy_game_objects.clear();
}

void scene::render_game_objects(
    std::chrono::duration<int, std::milli> delta_time)
{
    for (game_object* obj : game_objects)
    {
        obj->render(delta_time);
    }
}

void scene::on_event_game_objects(Kengine::event::game_event e)
{
    for (controller* c : controllers)
    {
        c->on_event(e);
    }
}

void scene::control_game_objects(
    std::chrono::duration<int, std::milli> delta_time)
{
    for (controller* c : controllers)
    {
        c->control(delta_time);
    }
    for (auto& destroy_controller : destroy_controllers)
    {
        auto remove_c = std::remove(
            controllers.begin(), controllers.end(), destroy_controller);
        if (remove_c != controllers.end())
        {
            controllers.erase(remove_c);
            delete destroy_controller;
        }
    }
    destroy_controllers.clear();
}

void scene::destroy_all_game_objects()
{
    for (auto& destroy_game_object : destroy_game_objects)
    {
        auto remove_go = std::remove(
            game_objects.begin(), game_objects.end(), destroy_game_object);
        if (remove_go != game_objects.end())
        {
            game_objects.erase(remove_go);
            delete destroy_game_object;
        }
    }
    destroy_game_objects.clear();
    for (auto& game_object : game_objects)
    {
        delete game_object;
    }
    game_objects.clear();
}

void scene::destroy_all_controllers()
{
    for (auto& destroy_controller : destroy_controllers)
    {
        auto remove_c = std::remove(
            controllers.begin(), controllers.end(), destroy_controller);
        if (remove_c != controllers.end())
        {
            controllers.erase(remove_c);
            delete destroy_controller;
        }
    }
    destroy_controllers.clear();
    for (auto& controller : controllers)
    {
        delete controller;
    }
    controllers.clear();
}
scene_state* scene::get_current_state()
{
    if (current_state < 0)
        return nullptr;

    return states[current_state];
}
scene::scene()
    : current_state(-1)
{
}

scene_state::~scene_state() = default;
