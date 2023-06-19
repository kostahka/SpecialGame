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
    static const std::string name;

    planet_scene();

    void on_start() override;

    ~planet_scene() override;

    enemy_spawner e_spawner;

private:
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