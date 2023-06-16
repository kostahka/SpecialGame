#pragma once

#include "Kengine/event/event.hxx"

#include "destroy_listener.hxx"

#include <chrono>
#include <ratio>
#include <vector>

class controller : public destroyable
{
public:
    virtual void control(std::chrono::duration<int, std::milli> delta_time) = 0;
    virtual void on_event(Kengine::event::game_event e)                     = 0;

    void destroy() override;

    controller();
    virtual ~controller();
};