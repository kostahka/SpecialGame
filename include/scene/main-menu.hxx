#pragma once

#include "scene.hxx"

class main_menu_scene : public scene
{
    friend class main_menu_state;

public:
    static const std::string name;

    main_menu_scene();

    void on_start() override;

    ~main_menu_scene() override;
};

class main_menu_state : public scene_state
{
public:
    static const std::string name;

    explicit main_menu_state();

    void on_event(const Kengine::event::game_event&) override;
    void update(std::chrono::duration<int, std::milli> delta_time) override;
    void render(std::chrono::duration<int, std::milli> delta_time) override;
    void imgui_render() override;
};
