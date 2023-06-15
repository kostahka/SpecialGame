#pragma once

#include "Kengine/event/event.hxx"

#include <chrono>
#include <ratio>

class game_object
{
public:
    virtual void on_event(Kengine::event::game_event e)                    = 0;
    virtual void input_process()                                           = 0;
    virtual void update(std::chrono::duration<int, std::milli> delta_time) = 0;
    virtual void render(std::chrono::duration<int, std::milli> delta_time) = 0;

    virtual void destroy() = 0;

    game_object();
    ~game_object();
};