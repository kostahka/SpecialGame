#include "astronaut/player.hxx"

#include "Kengine/io/input.hxx"
#include "game.hxx"
#include "physics/physics.hxx"
#include "render/camera.hxx"

constexpr int astronaut_id       = 0;
constexpr int player_reload_time = 200;

player::player(const Kengine::transform2d& pos)
    : dead(false)
    , selected_gun(1)
    , reload_time(0)
    , drill(false)
{
    player_astronaut = new astronaut(pos, false);
    player_astronaut->add_destroy_listener(this, astronaut_id);
};

void player::control(std::chrono::duration<int, std::milli> delta_time)
{
    if (reload_time > 0)
    {
        reload_time -= delta_time.count();
    }
    if (!dead)
    {
        Kengine::transform2d player_pos = player_astronaut->get_pos();
        camera::look_at(player_pos, player_pos - physics::land.get_center());

        using namespace Kengine::input;
#ifdef __ANDROID__
        if (current_game->move_joystick->active)
        {
            player_astronaut->move(current_game->move_joystick->axis_x);
            if (current_game->move_joystick->axis_y > 0.2)
            {
                player_astronaut->fly();
            }
        }

        if (current_game->aim_joystick->mod > 0.2)
        {
            float aim_angle = std::atan2(current_game->aim_joystick->axis_y,
                                         current_game->aim_joystick->axis_x);
            aim_angle += player_astronaut->get_angle();
            player_astronaut->aim(aim_angle);
            if (selected_gun == 1)
            {
                if (reload_time <= 0)
                {
                    player_astronaut->shoot();
                    reload_time = player_reload_time;
                }
            }
            else
            {
                if (!drill)
                {
                    player_astronaut->shoot();
                    drill = true;
                }
            }
        }
        else if (selected_gun == 2 && drill)
        {
            player_astronaut->shoot();
            drill = false;
        }
#else
        if (keyboard::key_pressed(keyboard::key::key_a))
        {
            player_astronaut->move(-1);
        }
        else if (keyboard::key_pressed(keyboard::key::key_d))
        {
            player_astronaut->move(1);
        }
        if (keyboard::key_pressed(keyboard::key::key_w))
        {
            player_astronaut->fly();
        }

        Kengine::transform2d mouse_pos =
            camera::screen_to_global({ mouse::x, mouse::y });

        Kengine::transform2d delta_pos =
            mouse_pos - player_astronaut->get_pos();
        float mouse_angle = std::atan2(delta_pos.y, delta_pos.x);
        player_astronaut->aim(mouse_angle);

        if (mouse::button_pressed(mouse::button::left))
        {
            if (selected_gun == 1)
            {
                if (reload_time <= 0)
                {
                    player_astronaut->shoot();
                    reload_time = player_reload_time;
                }
            }
            else
            {
                if (!drill)
                {
                    player_astronaut->shoot();
                    drill = true;
                }
            }
        }
        else if (selected_gun == 2 && drill)
        {
            player_astronaut->shoot();
            drill = false;
        }
#endif
        hp = player_astronaut->get_hp();
    }
}
void player::on_event(Kengine::event::game_event e)
{
    if (!dead)
    {
        using namespace Kengine::event;
        using namespace Kengine::input;
        if (e.g_type == type::keyboard_event && e.keyboard.pressed)
        {
            if (e.keyboard.key == keyboard::key::key_1)
            {
                current_game->game_cursor->set_cursor(cursor_type::attack);
                selected_gun = 1;
                drill        = false;
                player_astronaut->select_gun(gun_type::pistol);
            }
            else if (e.keyboard.key == keyboard::key::key_2)
            {
                current_game->game_cursor->set_cursor(cursor_type::shovel);
                selected_gun = 2;
                player_astronaut->select_gun(gun_type::drill);
            }
        }
    }
}

void player::on_destroy(int object_id)
{
    if (object_id == astronaut_id)
    {
        dead             = true;
        player_astronaut = nullptr;
        hp               = 0;
    }
}
astronaut* player::get_astronaut() const
{
    return player_astronaut;
}
int player::get_hp() const
{
    return hp;
}
int player::get_selected_gun() const
{
    return selected_gun;
}
void player::select_gun(int gun)
{
    if (gun == 1)
    {
        current_game->game_cursor->set_cursor(cursor_type::attack);
        selected_gun = 1;
        drill        = false;
        player_astronaut->select_gun(gun_type::pistol);
    }
    if (gun == 2)
    {
        current_game->game_cursor->set_cursor(cursor_type::shovel);
        selected_gun = 2;
        player_astronaut->select_gun(gun_type::drill);
    }
}
