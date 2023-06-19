#pragma once

#include "Kengine/event/event.hxx"
#include "game-object/controller.hxx"
#include "game-object/game-object.hxx"
#include <chrono>
#include <map>
#include <ratio>
#include <string>

class scene_state
{
public:
    virtual void on_event(const Kengine::event::game_event&)               = 0;
    virtual void update(std::chrono::duration<int, std::milli> delta_time) = 0;
    virtual void render(std::chrono::duration<int, std::milli> delta_time) = 0;
    virtual void imgui_render()                                            = 0;

    virtual ~scene_state();
};

class scene
{
public:
    scene();

    scene_state* get_current_state();

    virtual void on_start() = 0;

    void set_state(const std::string& name);
    void add_game_object(game_object* obj);
    void destroy_game_object(game_object* obj);

    void add_controller(controller* c);
    void destroy_controller(controller* c);

    void update_game_objects(std::chrono::duration<int, std::milli> delta_time);
    void render_game_objects(std::chrono::duration<int, std::milli> delta_time);

    void on_event_game_objects(Kengine::event::game_event e);
    void control_game_objects(
        std::chrono::duration<int, std::milli> delta_time);

    void destroy_all_game_objects();
    void destroy_all_controllers();

    virtual ~scene();

protected:
    std::map<std::string, int> state_id;
    std::vector<scene_state*>  states;
    int                        current_state;

    std::vector<game_object*> game_objects;
    std::vector<game_object*> destroy_game_objects;

    std::vector<controller*> controllers;
    std::vector<controller*> destroy_controllers;
};