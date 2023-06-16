#pragma once

#include "Kengine/engine.hxx"
#include "Kengine/render/b2GLDraw.hxx"
#include "glm/fwd.hpp"
#include <box2d/box2d.h>

#include "Kengine/render/sprite.hxx"
#include "Kengine/render/texture.hxx"
#include "game-object/game-object.hxx"
#include "landscape.hxx"
#include "render/cursor.hxx"
#include "scene/scene.hxx"

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
        std::chrono::duration<int, std::milli> delta_time) override;

    void on_imgui_render() override;

    void add_game_object(game_object* obj);
    void destroy_game_object(game_object* obj);

    void add_controller(controller* c);
    void destroy_controller(controller* c);

    ~my_game() override;

    cursor* game_cursor;

private:
    scene* current_scene;
};
extern my_game* current_game;

extern "C" game* create_game(engine* e);
