#pragma once

#include "Kengine/event/event.hxx"
#include "destroy_listener.hxx"

#include <chrono>
#include <ratio>

class game_object : public destroyable
{
public:
    virtual void update(std::chrono::duration<int, std::milli> delta_time) = 0;
    virtual void render(std::chrono::duration<int, std::milli> delta_time) = 0;

    void destroy() override;

    game_object();
    virtual ~game_object();
};
