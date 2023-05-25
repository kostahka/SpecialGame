#pragma once

#include "engine.hxx"
#include "glm/fwd.hpp"
#include "landscape.hxx"

using namespace Kengine;

class my_game : public game
{

public:
    my_game(engine* game_engine);

    std::chrono::high_resolution_clock::time_point start_point;

    void on_start() override;

    void on_event(event e) override;

    virtual void on_update(
        std::chrono::duration<int, std::milli> delta_time) override;

    virtual void on_render(
        std::chrono::duration<int, std::milli> delta_time) const override;

    ~my_game() override;

    static glm::mat4 projection;

private:
    landscape land;
};
