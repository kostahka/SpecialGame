#include "system/player-system.hxx"

#include "Kengine/components/transform-component.hxx"
#include "Kengine/io/input.hxx"
#include "Kengine/log/log.hxx"
#include "Kengine/scene/scene.hxx"

#include "components/astronaut-component.hxx"
#include "components/gui-selectable-component.hxx"
#include "components/player-controller.hxx"
#include "components/player-gun-selectable-component.hxx"
#include "components/player-health-bar-component.hxx"
#include "components/progress-bar-component.hxx"

static Kengine::string_id player_select_pistol_event_id{};
static Kengine::string_id player_select_drill_event_id{};

player_system::player_system(Kengine::scene& sc)
    : system(name, Kengine::system_event_type | Kengine::system_update_type)
    , sc(sc)
{
}

player_system::~player_system() {}

std::size_t player_system::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t player_system::deserialize(std::istream& is)
{
    return 0;
}

std::size_t player_system::serialize_size() const
{
    return 0;
}

void player_system::on_start(Kengine::scene&)
{
    sc.instansiate(Kengine::hash_string("player-gui"));
    player_select_pistol_event_id =
        Kengine::hash_string("player_select_pistol");
    player_select_drill_event_id = Kengine::hash_string("player_select_drill");
}

void player_system::on_event(Kengine::scene&                   sc,
                             const Kengine::event::game_event& g_event)
{

    using namespace Kengine::event;
    using namespace Kengine::input;
    if (g_event.g_type == type::keyboard_event && g_event.keyboard.pressed)
    {
        if (g_event.keyboard.key == keyboard::key::key_1)
        {
            select_player_gun(gun_type::pistol);
        }
        else if (g_event.keyboard.key == keyboard::key::key_2)
        {
            select_player_gun(gun_type::drill);
        }
    }
    else if (g_event.g_type == type::gui_event &&
             g_event.gui.mouse.button == Kengine::input::mouse::button::left &&
             g_event.gui.mouse.pressed)
    {
        if (g_event.gui.gui_id == player_select_pistol_event_id)
        {
            select_player_gun(gun_type::pistol);
        }
        else if (g_event.gui.gui_id == player_select_drill_event_id)
        {
            select_player_gun(gun_type::drill);
        }
    }
}

void player_system::on_render(Kengine::scene&, int delta_ms) {}

void player_system::on_update(Kengine::scene&, int delta_ms)
{
    players_count = 0;
    auto pl_view  = sc.registry.view<Kengine::transform_component,
                                    astronaut_component,
                                    player_controller>();

    auto pl_health_bar_view =
        sc.registry.view<player_health_bar_component, progress_bar_component>();

    float health = 0.f;
    for (auto [ent, trans_ent, astr_ent, pl_ent] : pl_view.each())
    {
        ++players_count;
        if (pl_ent.reload_time > 0)
        {
            pl_ent.reload_time -= delta_ms;
        }
        if (!pl_ent.dead)
        {
            health = astr_ent.hp;
            using namespace Kengine::input;

            if (keyboard::key_pressed(keyboard::key::key_a))
            {
                astr_ent.moving         = true;
                astr_ent.move_direction = -1;
            }
            else if (keyboard::key_pressed(keyboard::key::key_d))
            {
                astr_ent.moving         = true;
                astr_ent.move_direction = 1;
            }
            if (keyboard::key_pressed(keyboard::key::key_w))
            {
                astr_ent.flying = true;
            }

            Kengine::vec2 mouse_pos =
                sc.get_camera().screen_to_world({ mouse::x, mouse::y });

            Kengine::vec2 delta_pos   = mouse_pos - trans_ent.transf.position;
            float         mouse_angle = std::atan2(delta_pos.y, delta_pos.x);
            astr_ent.gun_angle        = mouse_angle;

            if (mouse::button_pressed(mouse::button::left))
            {
                if (pl_ent.selected_gun == 1)
                {
                    if (pl_ent.reload_time <= 0)
                    {
                        astr_ent.shoot = true;

                        pl_ent.reload_time = pl_ent.total_reload_time;
                    }
                }
                else
                {
                    if (!pl_ent.drill)
                    {
                        astr_ent.shoot = true;
                        pl_ent.drill   = true;
                    }
                }
            }
            else if (pl_ent.selected_gun == 2 && pl_ent.drill)
            {
                astr_ent.shoot = true;
                pl_ent.drill   = false;
            }
            pl_ent.hp = static_cast<int>(astr_ent.hp);
        }
    }
    health /= astronaut_hp;
    for (auto [ent, pl_health_bar_ent, pr_bar_ent] : pl_health_bar_view.each())
    {
        pr_bar_ent.progress = health;
    }
}

void player_system::select_player_gun(gun_type gun)
{
    int selected_gun = gun == gun_type::pistol ? 1 : 2;

    if (gun == gun_type::pistol)
    {
        // current_game->game_cursor->set_cursor(cursor_type::attack);
    }
    else if (gun == gun_type::drill)
    {
        // current_game->game_cursor->set_cursor(cursor_type::shovel);
    }

    auto pl_view = sc.registry.view<Kengine::transform_component,
                                    astronaut_component,
                                    player_controller>();
    for (auto [ent, trans_ent, astr_ent, pl_ent] : pl_view.each())
    {
        if (!pl_ent.dead)
        {
            pl_ent.selected_gun = selected_gun;
            astr_ent.select_gun(gun);
            if (gun == gun_type::pistol)
            {
                pl_ent.drill = false;
            }
        }
    }
    auto pl_gun_select_view =
        sc.registry
            .view<gui_selectable_component, player_gun_selectable_component>();

    for (auto [ent, gui_select_ent, pl_gun_select_ent] :
         pl_gun_select_view.each())
    {
        gui_select_ent.selected = pl_gun_select_ent.gun == gun;
    }
}
