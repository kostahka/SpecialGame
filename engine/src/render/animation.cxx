#include "Kengine/render/animation.hxx"

namespace Kengine
{

animation_controller::animation_controller(texture_object*    texture,
                                           const transform2d& pos,
                                           const transform2d& size)
    : anim_sprite(texture, { 0, 0, 0, 0 }, pos, size)
    , anim_id()
    , animations()
    , current_anim(-1)
    , play_anim(false)
    , play_one_shot_anim(false)
    , last_anim(-1)
    , anim_time(0)
{
}

void animation_controller::add_animation(std::string               name,
                                         const std::vector<irect>& a,
                                         int                       delta_time)
{
    anim_id[name] = ++last_anim;
    animations.push_back({ a, delta_time });
    if (current_anim == -1)
        current_anim = last_anim;
}

void animation_controller::set_current_animation(std::string name)
{
    if (anim_id.contains(name))
        current_anim = anim_id[name];
}

void animation_controller::play_one_shot()
{
    assert(current_anim != -1);
    play_anim          = true;
    play_one_shot_anim = true;
}

void animation_controller::play()
{
    assert(current_anim != -1);
    play_anim = true;
}

void animation_controller::stop()
{
    play_anim          = false;
    play_one_shot_anim = false;
}

void animation_controller::draw(
    std::chrono::duration<int, std::milli> delta_time)
{
    if (play_anim)
    {
        anim_time += delta_time.count();
        animation& current_animation = animations[current_anim];
        if (anim_time > current_animation.delta_time)
        {
            if (play_one_shot_anim && current_animation.current_frame ==
                                          current_animation.frames.size() - 1)
            {
                play_anim          = false;
                play_one_shot_anim = false;
            }

            current_animation.current_frame =
                (current_animation.frames.size() +
                 current_animation.current_frame + delta_frame) %
                static_cast<int>(current_animation.frames.size());
            anim_sprite.set_uv(
                current_animation.frames[current_animation.current_frame]);
            anim_time = 0;
        }
    }
    anim_sprite.draw();
}

void animation_controller::set_pos(const transform2d& pos)
{
    anim_sprite.set_pos(pos);
}

void animation_controller::set_angle(const float angle)
{
    anim_sprite.set_angle(angle);
}

void animation_controller::set_origin(const transform2d& origin)
{
    anim_sprite.set_origin(origin);
}

void animation_controller::set_size(const transform2d& size)
{
    anim_sprite.set_size(size);
}
void animation_controller::set_animation_delta_frame(int delta_frame)
{
    this->delta_frame = delta_frame;
}
} // namespace Kengine