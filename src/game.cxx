#include "game.hxx"

#include "engine.hxx"
#include "glm/ext/matrix_clip_space.hpp"
#include "imgui_internal.h"
#include "input.hxx"
#include "texture.hxx"

#include "camera.hxx"
#include "resources.hxx"

using namespace Kengine;

static bool debug_draw = false;

constexpr int physics_velocity_iterations = 10;
constexpr int physics_position_iterations = 8;

my_game* current_game{ nullptr };

std::chrono::high_resolution_clock game_clock;

my_game::my_game(engine* game_engine)
    : game("My game", game_engine)
    , land()
    , physics_world({ 0.0f, 0.0f }){};

void my_game::on_start()
{
    resources::init();

    start_point            = game_clock.now();
    float proj_width_half  = configuration.screen_width / 2.0f;
    float proj_height_half = configuration.screen_height / 2.0f;
    projection             = glm::ortho(-proj_width_half,
                            proj_width_half,
                            -proj_height_half,
                            proj_height_half,
                            -50.0f,
                            50.0f);
    view                   = glm::mat4(1);

    land.init();
    game_player = new player({ 50, 50 });
    game_cursor = new cursor();

    game_cursor->set_cursor(cursor_type::shovel);

    physics_draw.Create();
    uint32 debug_draw_flags = 0;
    // debug_draw_flags += b2Draw::e_aabbBit;
    // debug_draw_flags += b2Draw::e_centerOfMassBit;
    // debug_draw_flags += b2Draw::e_jointBit;
    // debug_draw_flags += b2Draw::e_pairBit;
    debug_draw_flags += b2Draw::e_shapeBit;
    physics_draw.SetFlags(debug_draw_flags);
    physics_world.SetDebugDraw(&physics_draw);

    game_engine->set_cursor_visible(false);
};

void my_game::on_event(event::game_event e){};

void my_game::on_update(std::chrono::duration<int, std::milli> delta_time)
{
    physics_world.Step(delta_time.count() / 1000.f,
                       physics_velocity_iterations,
                       physics_position_iterations);
    game_player->update(delta_time);
    using namespace Kengine::input;
    if (mouse::button_pressed(mouse::button::left))
    {
        float delta_value = delta_time.count() / 1'000.f;
        land.change_ground(mouse::x,
                           configuration.screen_height - mouse::y,
                           20.0,
                           delta_value);
    }
    if (mouse::button_pressed(mouse::button::right))
    {
        float delta_value = -delta_time.count() / 1'000.f;
        land.change_ground(mouse::x,
                           configuration.screen_height - mouse::y,
                           20.0,
                           delta_value);
    }
    transform2d player_pos = game_player->get_pos();
    camera::look_at(player_pos, player_pos - land.get_center());
};

void my_game::on_render(std::chrono::duration<int, std::milli> delta_time) const
{
    game_engine->clear_color({ 0.0, 0.0, 0.0, 0.0 });

    game_player->draw();

    land.draw();
    if (debug_draw)
    {
        const_cast<my_game*>(this)->physics_world.DebugDraw();
        const_cast<b2GLDraw*>(&physics_draw)->Draw();
    }

    game_engine->draw_imgui();

    game_cursor->draw();

    game_engine->swap_buffers();
};

void my_game::on_imgui_render()
{
    // clang-format off
    constexpr ImGuiWindowFlags game_gui_flags =
                                            0 |
                                            ImGuiWindowFlags_NoBackground |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoNav |
                                            ImGuiWindowFlags_NoBackground |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoScrollbar |
                                            ImGuiWindowFlags_NoScrollWithMouse |
                                            ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoTitleBar
;
    // clang-format on
    if (ImGui::Begin("Guns", 0, game_gui_flags))
    {
        ImGui::SetWindowPos({ 0, 0 });
        ImGui::Checkbox("Debug draw", &debug_draw);

        ImGui::End();
    }
    if (ImGui::Begin("Develop"))
    {
        ImGui::Checkbox("Debug draw", &debug_draw);
    }
    ImGui::End();

    ImGui::ShowDemoWindow();
};

my_game::~my_game(){};

game* create_game(engine* e)
{
    if (e != nullptr)
    {
        if (!current_game)
            current_game = new my_game(e);
        return current_game;
    }

    return nullptr;
};
