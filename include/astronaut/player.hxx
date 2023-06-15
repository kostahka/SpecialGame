#pragma once

#include "astronaut.hxx"

class player : public astronaut
{
public:
    explicit player(const Kengine::transform2d& pos);

    void on_event(Kengine::event::game_event e) override;
    void input_process() override;
};