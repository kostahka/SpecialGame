#include "astronaut/player.hxx"

#include "Kengine/io/input.hxx"
#include "physics/physics.hxx"
#include "render/camera.hxx"

constexpr int astronaut_id = 0;

player::player(const Kengine::transform2d& pos)
    : dead(false)
{
    player_astronaut = new astronaut(pos, false);
    player_astronaut->add_destroy_listener(this, astronaut_id);
};

void player::control(std::chrono::duration<int, std::milli> delta_time)
{
    if (!dead)
    {
        Kengine::transform2d player_pos = player_astronaut->get_pos();
        camera::look_at(player_pos, player_pos - physics::land.get_center());

        using namespace Kengine::input;
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
        float mouse_angle = std::atan2f(delta_pos.y, delta_pos.x);
        player_astronaut->aim(mouse_angle);

        hp = player_astronaut->get_hp();
    }
}
void player::on_event(Kengine::event::game_event e)
{
    if (!dead)
    {
        using namespace Kengine::event;
        using namespace Kengine::input;
        if (e.type == type::keyboard_event && e.keyboard.pressed)
        {
            if (e.keyboard.key == keyboard::key::key_1)
            {
                player_astronaut->select_gun(gun::pistol);
            }
            else if (e.keyboard.key == keyboard::key::key_2)
            {
                player_astronaut->select_gun(gun::drill);
            }
        }
        if (e.type == type::mouse_button_event && e.mouse.pressed)
        {
            if (e.mouse.button == mouse::button::left)
            {
                player_astronaut->shoot();
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
