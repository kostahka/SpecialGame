#pragma once

#include "Kengine/event/event.hxx"
#include "game-object/game-object.hxx"
#include <chrono>
#include <ratio>
#include <string>

class scene_state
{
public:
    virtual void on_event(const Kengine::event::game_event&)               = 0;
    virtual void update(std::chrono::duration<int, std::milli> delta_time) = 0;
    virtual void render(std::chrono::duration<int, std::milli> delta_time) = 0;
    virtual void imgui_render()                                            = 0;
};

class scene
{
public:
    virtual scene_state* get_current_state() = 0;

    virtual void on_start() = 0;

    virtual void set_state(const std::string& name)   = 0;
    virtual void add_game_object(game_object* obj)    = 0;
    virtual void remove_game_object(game_object* obj) = 0;

    virtual ~scene();
};