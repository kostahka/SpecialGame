#include "joystick.hxx"

#include "render/camera.hxx"
#include "resources.hxx"

joystick::joystick()
    : center_sprite(resources::joystick_texture,
                    { 2 * 128, 2 * 128, 128, 128 },
                    { 0, 0 },
                    { 0, 0 },
                    false,
                    1)
    , circle_sprite(resources::joystick_texture,
                    { 0 * 128, 2 * 128, 128 * 2, 128 * 2 },
                    { 0, 0 },
                    { 0, 0 },
                    false,
                    1)
    , active(false)
    , pos({ 0, 0 })
    , size(0)
    , axis_x(0)
    , axis_y(0)
    , mod(0)
{
    center_sprite.set_origin({ 0.5, 0.5 });
    circle_sprite.set_origin({ 0.5, 0.5 });
}
void joystick::draw()
{
    center_sprite.draw();
    circle_sprite.draw();
}
void joystick::on_event(const Kengine::event::game_event& event)
{
    using namespace Kengine::event;
    if (event.g_type == type::touch_event)
    {
        if (event.touch.pressed)
        {
            Kengine::transform2d touch_pos =
                camera::screen_to_local({ event.touch.x, event.touch.y });

            if (touch_pos.x < pos.x + size / 2 &&
                touch_pos.x > pos.x - size / 2 &&
                touch_pos.y < pos.y + size / 2 &&
                touch_pos.y > pos.y - size / 2)
            {
                touch_id  = event.touch.touch_id;
                finger_id = event.touch.finger_id;
                active    = true;
                center_sprite.set_uv({ 3 * 128, 2 * 128, 128, 128 });
            }
        }
        else
        {
            if (active && event.touch.touch_id == touch_id &&
                event.touch.finger_id == finger_id)
            {
                deactivate();
            }
        }
    }
    else if (event.g_type == type::touch_move_event)
    {
        if (active && event.touch.touch_id == touch_id &&
            event.touch.finger_id == finger_id)
        {
            float x = event.touch.x;
            float y = event.touch.y;

            Kengine::transform2d touch_pos = camera::screen_to_local({ x, y });
            axis_x                         = (touch_pos.x - pos.x) / size * 2;
            axis_y                         = (touch_pos.y - pos.y) / size * 2;
            mod = std::sqrt(axis_x * axis_x + axis_y * axis_y);

            if (mod > 1)
            {
                axis_x = axis_x / mod;
                axis_y = axis_y / mod;
            }

            center_sprite.set_pos(
                pos +
                Kengine::transform2d{ size / 2 * axis_x, size / 2 * axis_y });
        }
    }
}

void joystick::set_pos(const Kengine::transform2d& pos)
{
    this->pos = pos;
    center_sprite.set_pos(pos);
    circle_sprite.set_pos(pos);
}

void joystick::set_size(float size)
{
    this->size = size;
    center_sprite.set_size({ size / 2.f, size / 2.f });
    circle_sprite.set_size({ size, size });
}
void joystick::deactivate()
{
    active = false;
    center_sprite.set_uv({ 2 * 128, 2 * 128, 128, 128 });
    axis_x = 0;
    axis_y = 0;
    mod    = 0;
    center_sprite.set_pos(pos);
}
