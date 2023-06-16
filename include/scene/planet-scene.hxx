#pragma once

#include "astronaut/enemy_spawner.hxx"
#include "astronaut/player.hxx"
#include "landscape.hxx"
#include "scene.hxx"

#include <map>
#include <vector>

class planet_scene : public scene
{
    friend class planet_start_state;
    friend class planet_play_state;
    friend class planet_end_state;

public:
    planet_scene();

    void         on_start() override;
    scene_state* get_current_state() override;
    void         set_state(const std::string& name) override;

    void add_game_object(game_object* obj) override;
    void destroy_game_object(game_object* obj) override;

    void add_controller(controller* c) override;
    void destroy_controller(controller* c) override;

    void update_game_objects(
        std::chrono::duration<int, std::milli> delta_time) override;
    void render_game_objects(
        std::chrono::duration<int, std::milli> delta_time) override;

    void on_event_game_objects(Kengine::event::game_event e) override;
    void control_game_objects(
        std::chrono::duration<int, std::milli> delta_time) override;

    ~planet_scene() override;

    enemy_spawner e_spawner;

private:
    std::map<std::string, int> state_id;
    std::vector<scene_state*>  states;
    int                        current_state;

    std::vector<game_object*> game_objects;
    std::vector<game_object*> destroy_game_objects;

    std::vector<controller*> controllers;
    std::vector<controller*> destroy_controllers;

    player* game_player;
};

class planet_start_state : public scene_state
{
public:
    static const std::string name;

    explicit planet_start_state(planet_scene* scene);

    void on_event(const Kengine::event::game_event&) override;
    void update(std::chrono::duration<int, std::milli> delta_time) override;
    void render(std::chrono::duration<int, std::milli> delta_time) override;
    void imgui_render() override;

private:
    planet_scene* state_scene;
};

class planet_play_state : public scene_state
{
public:
    static const std::string name;

    explicit planet_play_state(planet_scene* scene);

    void on_event(const Kengine::event::game_event&) override;
    void update(std::chrono::duration<int, std::milli> delta_time) override;
    void render(std::chrono::duration<int, std::milli> delta_time) override;
    void imgui_render() override;

private:
    planet_scene* state_scene;
};

class planet_end_state : public scene_state
{
public:
    static const std::string name;

    explicit planet_end_state(planet_scene* scene);

    void on_event(const Kengine::event::game_event&) override;
    void update(std::chrono::duration<int, std::milli> delta_time) override;
    void render(std::chrono::duration<int, std::milli> delta_time) override;
    void imgui_render() override;

private:
    planet_scene* state_scene;
};