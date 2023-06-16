#include "scene/planet-scene.hxx"
#include "astronaut/enemy.hxx"
#include "game.hxx"
#include "imgui.h"
#include "physics/physics.hxx"
#include "render/camera.hxx"
#include "render/game_gui.hxx"

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
void planet_scene::destroy_game_object(game_object* obj)
{
    destroy_game_objects.push_back(obj);
}

void planet_scene::on_start()
{
    physics::land.init();

    game_player = new player(physics::land.get_spawn_place(0));
    e_spawner.init(game_player->get_astronaut());
}
void planet_scene::update_game_objects(
    std::chrono::duration<int, std::milli> delta_time)
{
    for (game_object* obj : game_objects)
    {
        obj->update(delta_time);
    }
    for (int i = 0; i < destroy_game_objects.size(); i++)
    {
        game_objects.erase(std::remove(
            game_objects.begin(), game_objects.end(), destroy_game_objects[i]));
        delete destroy_game_objects[i];
    }
    destroy_game_objects.clear();
}

void planet_scene::add_controller(controller* c)
{
    controllers.push_back(c);
}

void planet_scene::destroy_controller(controller* c)
{
    destroy_controllers.push_back(c);
}

void planet_scene::render_game_objects(
    std::chrono::duration<int, std::milli> delta_time)
{
    for (game_object* obj : game_objects)
    {
        obj->render(delta_time);
    }
}

void planet_scene::on_event_game_objects(Kengine::event::game_event e)
{
    for (controller* c : controllers)
    {
        c->on_event(e);
    }
}

void planet_scene::control_game_objects(
    std::chrono::duration<int, std::milli> delta_time)
{
    for (controller* c : controllers)
    {
        c->control(delta_time);
    }
    for (int i = 0; i < destroy_controllers.size(); i++)
    {
        controllers.erase(std::remove(
            controllers.begin(), controllers.end(), destroy_controllers[i]));
        delete destroy_controllers[i];
    }
    destroy_controllers.clear();
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
    state_scene->on_event_game_objects(e);
}

void planet_play_state::update(
    std::chrono::duration<int, std::milli> delta_time)
{
    physics::update(delta_time);

    state_scene->e_spawner.update(delta_time);

    state_scene->update_game_objects(delta_time);
    state_scene->control_game_objects(delta_time);
}

void planet_play_state::render(
    std::chrono::duration<int, std::milli> delta_time)
{
    state_scene->render_game_objects(delta_time);

    physics::land.draw();
}

void planet_play_state::imgui_render()
{
    if (ImGui::Begin("Guns", nullptr, gui::window_flags))
    {
        if (ImGui::BeginChildFrame(1, { 100, 100 }))
        {

            ImGui::EndChildFrame();
        }

        ImGui::End();
    }

    ImGui::SetNextWindowPos(
        { static_cast<float>(current_game->configuration.screen_width - 60),
          60 });
    ImGui::SetNextWindowSize({ 60, 200 });
    if (ImGui::Begin("HP", nullptr, gui::window_flags))
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImGui::Text("Health");
        ImVec2 health_bar_size = ImVec2(30, 160);

        int   health = state_scene->game_player->get_hp();
        float fill   = health / 100.f;

        ImVec2 p0    = ImGui::GetCursorScreenPos();
        ImVec2 p1    = ImVec2(p0.x + health_bar_size.x,
                           p0.y + health_bar_size.y * (1 - fill));
        ImVec2 p2    = ImVec2(p0.x, p0.y + health_bar_size.y);
        ImU32  col_r = ImGui::GetColorU32(IM_COL32(255, 0, 0, 255));
        ImU32  col_g = ImGui::GetColorU32(IM_COL32(0, 255, 0, 255));
        draw_list->AddRectFilledMultiColor(p0, p1, col_r, col_r, col_r, col_r);
        draw_list->AddRectFilledMultiColor(p1, p2, col_g, col_g, col_g, col_g);

        ImGui::End();
    }
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
}

void planet_end_state::imgui_render() {}