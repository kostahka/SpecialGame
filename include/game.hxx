#pragma once

#include "Kengine/engine.hxx"

class special_game : public Kengine::game
{
public:
    special_game();
    ~special_game() override;

    void on_start() override;
    void on_event(const Kengine::event::game_event&) override;
    void on_update(int delta_ms) override;
    void on_render(int delta_ms) override;

    static special_game* instance;
};
