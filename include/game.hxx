#pragma once

#include "b2GLDraw.hxx"
#include "engine.hxx"
#include "glm/fwd.hpp"
#include <box2d/box2d.h>

#include "cursor.hxx"
#include "landscape.hxx"
#include "player.hxx"
#include "sprite.hxx"
#include "texture.hxx"

using namespace Kengine;

class my_game : public game
{

public:
    my_game(engine* game_engine);

    std::chrono::high_resolution_clock::time_point start_point;

    void on_start() override;

    void on_event(event::game_event e) override;

    virtual void on_update(
        std::chrono::duration<int, std::milli> delta_time) override;

    virtual void on_render(
        std::chrono::duration<int, std::milli> delta_time) const override;

    void on_imgui_render() override;

    ~my_game() override;

    b2World   physics_world;
    landscape land;
    b2GLDraw  physics_draw;
    cursor*   game_cursor;

private:
    player* game_player;
};
extern my_game* current_game;

extern "C" game* create_game(engine* e);
