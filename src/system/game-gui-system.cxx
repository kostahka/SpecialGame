#include "system/game-gui-system.hxx"

#include "Kengine/components/gui-component.hxx"
#include "Kengine/components/rect-transform-component.hxx"
#include "Kengine/io/input.hxx"
#include "Kengine/log/log.hxx"
#include "Kengine/scene/scene.hxx"

#include "components/gui-button-component.hxx"
#include "components/gui-selectable-component.hxx"
#include "components/progress-bar-component.hxx"

game_gui_system::game_gui_system(Kengine::scene& sc)
    : system(name, Kengine::system_render_type)
    , sc(sc)
{
}

game_gui_system::~game_gui_system() {}

std::size_t game_gui_system::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t game_gui_system::deserialize(std::istream& is)
{
    return 0;
}

std::size_t game_gui_system::serialize_size() const
{
    return 0;
}

void game_gui_system::on_start(Kengine::scene&) {}

void game_gui_system::on_event(Kengine::scene&,
                               const Kengine::event::game_event&)
{
}

void game_gui_system::on_render(Kengine::scene& sc, int delta_ms)
{
    auto progress_bar_view =
        sc.registry.view<Kengine::gui_component, progress_bar_component>();

    for (auto [ent, gui_ent, pr_bar_ent] : progress_bar_view.each())
    {
        float         t = glm::clamp(pr_bar_ent.progress, 0.0f, 1.0f);
        Kengine::vec2 current_uv_size =
            glm::mix(glm::vec2(pr_bar_ent.uv_size_start),
                     glm::vec2(pr_bar_ent.uv_size_end),
                     t);
        Kengine::vec2 current_scale =
            glm::mix(glm::vec2(pr_bar_ent.scale_start),
                     glm::vec2(pr_bar_ent.scale_end),
                     t);

        gui_ent.uv.w  = current_uv_size.x;
        gui_ent.uv.h  = current_uv_size.y;
        gui_ent.scale = current_scale;
    }

    auto select_view = sc.registry.view<Kengine::rect_transform_component,
                                        Kengine::gui_component,
                                        gui_selectable_component>();

    for (auto [ent, rect_trans_ent, gui_ent, select_ent] : select_view.each())
    {
        auto          world_transform = rect_trans_ent.get_world_transform();
        Kengine::vec2 rt = world_transform.start + world_transform.rect;

        if (select_ent.selected)
        {
            gui_ent.uv = select_ent.selected_uv;
        }
        else if (gui_ent.is_hovered)
        {
            gui_ent.uv = select_ent.hovered_uv;
        }
        else
        {
            gui_ent.uv = select_ent.default_uv;
        }
    }

    auto button_view = sc.registry.view<Kengine::rect_transform_component,
                                        Kengine::gui_component,
                                        gui_button_component>();
    for (auto [ent, rect_trans_ent, gui_ent, btn_ent] : button_view.each())
    {
        if (gui_ent.is_hovered)
        {
            if (Kengine::input::mouse::button_pressed(
                    Kengine::input::mouse::button::left))
            {
                gui_ent.uv = btn_ent.click_uv;
            }
            else
            {
                gui_ent.uv = btn_ent.hovered_uv;
            }
        }
        else
        {
            gui_ent.uv = btn_ent.default_uv;
        }
    }
}

void game_gui_system::on_update(Kengine::scene&, int delta_ms) {}