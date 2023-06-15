#include "astronaut/player.hxx"

#include "Kengine/io/input.hxx"
#include "render/camera.hxx"

player::player(const Kengine::transform2d& pos)
    : astronaut(pos, false){};

void player::input_process()
{
    using namespace Kengine::input;
    if (keyboard::key_pressed(keyboard::key::key_a))
    {
        astronaut::move(-1);
    }
    else if (keyboard::key_pressed(keyboard::key::key_d))
    {
        astronaut::move(1);
    }
    if (keyboard::key_pressed(keyboard::key::key_w))
    {
        astronaut::fly();
    }
    Kengine::transform2d mouse_pos =
        camera::screen_to_global({ mouse::x, mouse::y });

    Kengine::transform2d delta_pos   = mouse_pos - get_pos();
    float                mouse_angle = std::atan2f(delta_pos.y, delta_pos.x);
    aim(mouse_angle);
}
void player::on_event(Kengine::event::game_event e)
{
    using namespace Kengine::event;
    using namespace Kengine::input;
    if (e.type == type::keyboard_event && e.keyboard.pressed)
    {
        if (e.keyboard.key == keyboard::key::key_1)
        {
            select_gun(gun::pistol);
        }
        else if (e.keyboard.key == keyboard::key::key_2)
        {
            select_gun(gun::drill);
        }
    }
    if (e.type == type::mouse_button_event && e.mouse.pressed)
    {
        if (e.mouse.button == mouse::button::left)
        {
            shoot();
        }
    }
}
