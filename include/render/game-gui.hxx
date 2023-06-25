#pragma once

#include "imgui.h"

namespace gui
{
extern float gui_scale;
// clang-format off
    constexpr ImGuiWindowFlags window_flags =
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoNav |
//                                            ImGuiWindowFlags_NoBackground |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoScrollbar |
                                            ImGuiWindowFlags_NoScrollWithMouse |
                                            ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoTitleBar |
                                            ImGuiWindowFlags_AlwaysAutoResize
;
// clang-format on

void debug_draw_menu();
void draw_health_bar(int hp);
int  draw_selected_gun(int selected_gun);
void draw_score(int score);
void draw_training_hint();
bool draw_end_menu(int score);
bool draw_main_menu();
int  draw_pause_menu();
void draw_wave_time(int current, int max);
void draw_enemies_count(int current, int max);
} // namespace gui