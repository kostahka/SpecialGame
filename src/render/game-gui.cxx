#include "render/game-gui.hxx"
#include "game.hxx"
#include "render/resources.hxx"

namespace gui
{

bool ButtonCentered(const char* label, const ImVec2& size = { 0, 0 })
{
    float label_indentation;
    float win_width = ImGui::GetWindowSize().x;

    if (size.x == 0 || size.y == 0)
    {
        float label_width = ImGui::CalcTextSize(label).x;
        label_indentation = (win_width - label_width) * 0.5f;
    }
    else
    {
        label_indentation = (win_width - size.x) * 0.5f;
    }

    ImGui::SetCursorPosX(label_indentation);
    return ImGui::Button(label, size);
}

void TextCentered(const std::string& text)
{
    float win_width  = ImGui::GetWindowSize().x;
    float text_width = ImGui::CalcTextSize(text.c_str()).x;

    float text_indentation = (win_width - text_width) * 0.5f;

    float min_indentation = 20.0f;
    if (text_indentation <= min_indentation)
    {
        text_indentation = min_indentation;
    }

    ImGui::SetCursorPosX(text_indentation);
    ImGui::PushTextWrapPos(win_width - text_indentation);
    ImGui::TextWrapped("%s", text.c_str());
    ImGui::PopTextWrapPos();
}

void draw_health_bar(int hp)
{
    ImGui::SetNextWindowPos(
        { static_cast<float>(current_game->configuration.screen_width - 60),
          60 });
    ImGui::SetNextWindowSize({ ImGui::GetWindowSize().x, 200 });
    if (ImGui::Begin("HP", nullptr, gui::window_flags))
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImGui::Text("Health");
        ImVec2 health_bar_size = ImVec2(30, 160);

        float fill = static_cast<float>(hp) / 100.f;

        ImVec2 p0    = ImGui::GetCursorScreenPos();
        ImVec2 p1    = ImVec2(p0.x + health_bar_size.x,
                           p0.y + health_bar_size.y * (1 - fill));
        ImVec2 p2    = ImVec2(p0.x, p0.y + health_bar_size.y);
        ImU32  col_r = ImGui::GetColorU32(IM_COL32(255, 0, 0, 255));
        ImU32  col_g = ImGui::GetColorU32(IM_COL32(0, 255, 0, 255));
        draw_list->AddRectFilledMultiColor(p0, p1, col_r, col_r, col_r, col_r);
        draw_list->AddRectFilledMultiColor(p1, p2, col_g, col_g, col_g, col_g);

        ImGui::End();
    }
}

void draw_selected_gun(int selected_gun)
{
    if (ImGui::Begin("Guns", nullptr, gui::window_flags))
    {
        if (selected_gun == 1)
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg,
                                  (ImVec4)ImColor(IM_COL32(55, 55, 155, 255)));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg,
                                  (ImVec4)ImColor(IM_COL32(55, 55, 55, 255)));
        }

        if (ImGui::BeginChildFrame(1, { 84, 69 }))
        {

            ImGui::Text("1. Pistol");
            ImGui::Image(reinterpret_cast<void*>(
                             resources::special_game_texture->get_id()),
                         ImVec2(64, 44),
                         ImVec2(3 * 64 / 640.f, (2 * 64 + 10) / 640.f),
                         ImVec2(4 * 64 / 640.f, (3 * 64 - 10) / 640.f));

            ImGui::EndChildFrame();
        }
        ImGui::SameLine();
        ImGui::PopStyleColor(1);

        if (selected_gun == 2)
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg,
                                  (ImVec4)ImColor(IM_COL32(55, 55, 155, 255)));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg,
                                  (ImVec4)ImColor(IM_COL32(55, 55, 55, 255)));
        }

        if (ImGui::BeginChildFrame(2, { 84, 69 }))
        {
            ImGui::Text("2. Drill");
            ImGui::Image(reinterpret_cast<void*>(
                             resources::special_game_texture->get_id()),
                         ImVec2(64, 44),
                         ImVec2(2 * 64 / 640.f, (2 * 64 + 10) / 640.f),
                         ImVec2(3 * 64 / 640.f, (3 * 64 - 10) / 640.f));

            ImGui::EndChildFrame();
        }
        ImGui::PopStyleColor(1);

        ImGui::End();
    }
}
void draw_score(int score)
{
    ImGui::SetNextWindowPos(
        { static_cast<float>(current_game->configuration.screen_width / 2 - 50),
          50 });
    if (ImGui::Begin("Score", nullptr, gui::window_flags))
    {
        ImGui::Text("Score: %d", score);
        ImGui::End();
    }
}

void draw_training_hint()
{
    ImGui::SetNextWindowPos(
        { static_cast<float>(current_game->configuration.screen_width / 2 -
                             200),
          static_cast<float>(current_game->configuration.screen_height / 2 -
                             50) });
    ImGui::SetNextWindowSize({ 400, 100 });

    if (ImGui::Begin("Training", nullptr, gui::window_flags))
    {

        TextCentered("Move horizontal with [a], [d].\n"
                     "Fly with [w].\n"
                     "Shoot with [left mouse]\n"
                     "Choose gun with [1], [2].\n\n"
                     "Press any button to play...");
        ImGui::End();
    }
}

bool draw_end_menu(int score)
{
    bool restart = false;
    ImGui::SetNextWindowPos(
        { static_cast<float>(current_game->configuration.screen_width / 2 -
                             200),
          static_cast<float>(current_game->configuration.screen_height / 2 -
                             50) });
    ImGui::SetNextWindowSize({ 400, 120 });

    if (ImGui::Begin("End game", nullptr, gui::window_flags))
    {
        ImGui::SetWindowFontScale(2);

        TextCentered("Game Over");
        ImGui::SetWindowFontScale(1);
        std::string score_str = "Score: " + std::to_string(score);
        TextCentered(score_str);

        restart = ButtonCentered("Restart");

        if (ButtonCentered("Exit"))
        {
            current_game->game_engine->quit();
        }

        ImGui::End();
    }

    return restart;
}

bool draw_main_menu()
{
    bool start_game = false;
    ImGui::SetNextWindowPos(
        { static_cast<float>(current_game->configuration.screen_width / 2 -
                             200),
          static_cast<float>(current_game->configuration.screen_height / 2 -
                             50) });
    ImGui::SetNextWindowSize({ 400, 120 });

    if (ImGui::Begin("Main menu", nullptr, gui::window_flags))
    {
        ImGui::SetWindowFontScale(2);

        TextCentered("Main menu");
        ImGui::SetWindowFontScale(1);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
        start_game = ButtonCentered("Play", { 100, 20 });
        if (ButtonCentered("Exit", { 100, 20 }))
        {
            current_game->game_engine->quit();
        }

        ImGui::End();
    }
    return start_game;
}

} // namespace gui