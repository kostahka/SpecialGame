#include "system/player-system.hxx"

#include "Kengine/components/transform-component.hxx"
#include "Kengine/io/input.hxx"
#include "Kengine/scene/scene.hxx"

#include "components/astronaut-component.hxx"
#include "components/player-controller.hxx"

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

void player_system::on_start(Kengine::scene&) {}

void player_system::on_event(Kengine::scene&                   sc,
                             const Kengine::event::game_event& g_event)
{

    using namespace Kengine::event;
    using namespace Kengine::input;
    if (g_event.g_type == type::keyboard_event && g_event.keyboard.pressed)
    {
        auto pl_view = sc.registry.view<Kengine::transform_component,
                                        astronaut_component,
                                        player_controller>();
        for (auto [ent, trans_ent, astr_ent, pl_ent] : pl_view.each())
        {
            if (!pl_ent.dead)
            {
                if (g_event.keyboard.key == keyboard::key::key_1)
                {
                    // current_game->game_cursor->set_cursor(cursor_type::attack);
                    pl_ent.selected_gun  = 1;
                    pl_ent.drill         = false;
                    astr_ent.current_gun = gun_type::pistol;
                }
                else if (g_event.keyboard.key == keyboard::key::key_2)
                {
                    // current_game->game_cursor->set_cursor(cursor_type::shovel);
                    pl_ent.selected_gun  = 2;
                    astr_ent.current_gun = gun_type::drill;
                }
            }
        }
    }
}

void player_system::on_render(Kengine::scene&, int delta_ms) {}

void player_system::on_update(Kengine::scene&, int delta_ms)
{
    auto pl_view = sc.registry.view<Kengine::transform_component,
                                    astronaut_component,
                                    player_controller>();
    for (auto [ent, trans_ent, astr_ent, pl_ent] : pl_view.each())
    {
        if (pl_ent.reload_time > 0)
        {
            pl_ent.reload_time -= delta_ms;
        }
        if (!pl_ent.dead)
        {
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
                        astr_ent.shoot();
                        pl_ent.reload_time = pl_ent.total_reload_time;
                    }
                }
                else
                {
                    if (!pl_ent.drill)
                    {
                        astr_ent.shoot();
                        pl_ent.drill = true;
                    }
                }
            }
            else if (pl_ent.selected_gun == 2 && pl_ent.drill)
            {
                astr_ent.shoot();
                pl_ent.drill = false;
            }
            pl_ent.hp = static_cast<int>(astr_ent.hp);
        }
    }
}