#pragma once

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
    void         add_game_object(game_object* obj) override;
    void         remove_game_object(game_object* obj) override;

    ~planet_scene() override;

private:
    std::map<std::string, int> state_id;
    std::vector<scene_state*>  states;
    int                        current_state;

    std::vector<game_object*> game_objects;

    player* game_player;
};

class planet_start_state : public scene_state
{
public:
    static const std::string name;

    planet_start_state(planet_scene* scene);

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

    planet_play_state(planet_scene* scene);

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

    planet_end_state(planet_scene* scene);

    void on_event(const Kengine::event::game_event&) override;
    void update(std::chrono::duration<int, std::milli> delta_time) override;
    void render(std::chrono::duration<int, std::milli> delta_time) override;
    void imgui_render() override;

private:
    planet_scene* state_scene;
};