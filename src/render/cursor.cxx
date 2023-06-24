#include "render/cursor.hxx"

#include "render/camera.hxx"
#include "resources.hxx"

#include "Kengine/io/input.hxx"
#include "Kengine/transform3d.hxx"

cursor::cursor()
    : aim_sprite(resources::crosshairs_texture,
                 { 0, 0, 64, 64 },
                 { 0, 0 },
                 { 40, 40 },
                 false)
{
    aim_sprite.set_origin({ 0.5, 0.5 });
    aim_sprite.set_angle(0);
}
void cursor::draw() const
{
    Kengine::transform2d aim_pos = camera::screen_to_local(
        { Kengine::input::mouse::x, Kengine::input::mouse::y });
    const_cast<Kengine::sprite*>(&aim_sprite)->set_pos(aim_pos);

    aim_sprite.draw();
}

void cursor::set_cursor(cursor_type cur)
{
    switch (cur)
    {
        case cursor_type::simple:
            aim_sprite.set_uv({ 0, 0, 64, 64 });
            break;
        case cursor_type::shovel:
            aim_sprite.set_uv({ 2 * 64, 3 * 64, 64, 64 });
            break;
        case cursor_type::attack:
            aim_sprite.set_uv({ 192, 128, 64, 64 });
            break;
    }
}
