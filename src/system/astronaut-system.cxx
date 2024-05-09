#include "system/astronaut-system.hxx"

#include "Kengine/components/animation-component.hxx"
#include "Kengine/components/audio-component.hxx"
#include "Kengine/components/physics-component.hxx"
#include "Kengine/components/transform-component.hxx"
#include "Kengine/log/log.hxx"
#include "Kengine/scene/scene.hxx"

#include "components/astronaut-component.hxx"

#include "box2d/b2_fixture.h"

static Kengine::string_id astronaut_idle_animation_id{};
static Kengine::string_id astronaut_walk_animation_id{};
static Kengine::string_id astronaut_fly_animation_id{};

astronaut_system::astronaut_system(Kengine::scene& sc)
    : system(name, Kengine::system_update_type)
    , sc(sc)
{
}

astronaut_system::~astronaut_system() {}

std::size_t astronaut_system::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t astronaut_system::deserialize(std::istream& is)
{
    return 0;
}

std::size_t astronaut_system::serialize_size() const
{
    return 0;
}

void astronaut_system::on_start(Kengine::scene&)
{
    astronaut_idle_animation_id = Kengine::hash_string("idle");
    astronaut_walk_animation_id = Kengine::hash_string("walk");
    astronaut_fly_animation_id  = Kengine::hash_string("fly");
}

void astronaut_system::on_render(Kengine::scene&, int delta_ms) {}

void astronaut_system::on_update(Kengine::scene& sc, int delta_ms)
{
    auto astr_view = sc.registry.view<Kengine::transform_component,
                                      Kengine::physics_component,
                                      Kengine::animation_component,
                                      astronaut_component>();

    for (auto [ent, trans_ent, ph_ent, anim_ent, astr_ent] : astr_view.each())
    {
        float         astronaut_angle = trans_ent.transf.angle;
        Kengine::vec2 astronaut_pos   = trans_ent.transf.position;
        float         down_angle      = astronaut_angle - 3.14f / 2.f;

        astr_ent.on_ground = false;
        sc.get_world().RayCast(
            &astr_ent.ground_ray_cast_callback,
            astronaut_pos,
            { astronaut_pos.x +
                  std::cos(down_angle) * trans_ent.transf.scale.y / 1.75f,
              astronaut_pos.y +
                  std::sin(down_angle) * trans_ent.transf.scale.y / 1.75f });

        process_animation(astr_ent, anim_ent);

        if (astr_ent.walk_sound_entity != entt::null)
        {
            if (sc.registry.any_of<Kengine::audio_component>(
                    astr_ent.walk_sound_entity))
            {
                auto& walk_audio_comp =
                    sc.registry.get<Kengine::audio_component>(
                        astr_ent.walk_sound_entity);
                if (astr_ent.moving && astr_ent.on_ground)
                {
                    walk_audio_comp.resume();
                }
                else
                {
                    walk_audio_comp.pause();
                }
            }
        }

        if (astr_ent.moving)
        {
            const float delta_impulse =
                static_cast<float>(delta_ms) * astr_ent.move_speed;
            const b2Vec2 d_impulse_vec(
                -delta_impulse * astr_ent.to_earth_center.y,
                delta_impulse * astr_ent.to_earth_center.x);

            ph_ent->ApplyLinearImpulseToCenter(
                astr_ent.move_direction * d_impulse_vec, true);
            if (ph_ent.get_fixtures().size() > 0)
            {
                ph_ent.get_fixtures().back()->SetFriction(0.1f);
            }
            astr_ent.moving = false;
        }
        else
        {
            if (ph_ent.get_fixtures().size() > 0)
            {
                ph_ent.get_fixtures().back()->SetFriction(1.0f);
            }
        }

        if (astr_ent.fly_sound_entity != entt::null)
        {
            if (sc.registry.any_of<Kengine::audio_component>(
                    astr_ent.fly_sound_entity))
            {
                auto& fly_audio_comp =
                    sc.registry.get<Kengine::audio_component>(
                        astr_ent.fly_sound_entity);
                if (astr_ent.flying)
                {
                    fly_audio_comp.resume();
                }
                else
                {
                    fly_audio_comp.pause();
                }
            }
        }

        if (astr_ent.flying)
        {
            const float delta_fly_impulse =
                static_cast<float>(delta_ms) * astr_ent.fly_speed;
            const b2Vec2 d_fly_vec(
                delta_fly_impulse * -astr_ent.to_earth_center.x,
                delta_fly_impulse * -astr_ent.to_earth_center.y);

            ph_ent->ApplyLinearImpulseToCenter(d_fly_vec, true);
            astr_ent.flying = false;
        }

        // astronaut_anim.set_angle(astronaut_angle);
        // astronaut_anim.set_pos({ astronaut_pos.x, astronaut_pos.y });

        float d_gun_angle;
        int   direction;
        if (std::cos(astronaut_angle - astr_ent.gun_angle) < 0)
        {
            d_gun_angle = astr_ent.gun_angle - 3.14f;
            direction   = -1;
        }
        else
        {
            d_gun_angle = astr_ent.gun_angle;
            direction   = 1;
        }

        anim_ent.reversed = astr_ent.move_direction != direction;

        trans_ent.transf.scale.x =
            glm::abs(trans_ent.transf.scale.x) * static_cast<float>(direction);

        if (astr_ent.hand_anchor_entity != entt::null)
        {
            if (sc.registry.any_of<Kengine::transform_component>(
                    astr_ent.hand_anchor_entity))
            {
                auto& hand_anchor_transform =
                    sc.registry.get<Kengine::transform_component>(
                        astr_ent.hand_anchor_entity);

                hand_anchor_transform.transf.angle = d_gun_angle;
            }
        }

        // drill_beam_sprite.set_pos(
        //     { astronaut_pos.x + drill_beam_start_distance *
        //     std::cos(gun_angle),
        //       astronaut_pos.y +
        //           drill_beam_start_distance * std::sin(gun_angle) });
        // drill_beam_sprite.set_angle(gun_angle);
        // if (drilling)
        // {
        //     drill_beam_sprite.set_size({ drill_max_distance, drill_size });

        //     drill_ray_cast_callback.drill_collision_info = 0;
        //     physics::physics_world.RayCast(
        //         &drill_ray_cast_callback,
        //         astronaut_pos,
        //         { astronaut_pos.x + std::cos(gun_angle) * drill_max_distance,
        //           astronaut_pos.y + std::sin(gun_angle) * drill_max_distance
        //           });

        //     if (drill_ray_cast_callback.drill_collision_info)
        //     {
        //         auto collision_object =
        //         reinterpret_cast<collision_interface*>(
        //             drill_ray_cast_callback.drill_collision_info);
        //         if (collision_object->hurt(
        //                 drill_damage_radius,
        //                 drill_damage * delta_time.count(),
        //                 { drill_ray_cast_callback.drill_collision_point.x,
        //                   drill_ray_cast_callback.drill_collision_point.y },
        //                 gun_type::drill))
        //         {
        //             if (!ground_drilling_sound->get_is_playing())
        //             {
        //                 ground_drilling_sound->play();
        //             }
        //         }

        //         collision_object->hurt((int)drill_damage *
        //         delta_time.count());

        //         drill_beam_sprite.set_size(
        //             { drill_ray_cast_callback.drill_distance -
        //                   drill_beam_start_distance,
        //               drill_size });
        //     }
        //     else
        //     {
        //         ground_drilling_sound->stop();
        //     }
        // }
    }
}

void astronaut_system::process_animation(astronaut_component&          astr_ent,
                                         Kengine::animation_component& anim_ent)
{
    auto curr_anim_id = anim_ent.get_current_animation();
    if (curr_anim_id == astronaut_idle_animation_id)
    {
        if (astr_ent.flying)
        {
            anim_ent.set_current_animation(astronaut_fly_animation_id);
        }
        else if (astr_ent.moving)
        {
            anim_ent.set_current_animation(astronaut_walk_animation_id);
        }
    }
    else if (curr_anim_id == astronaut_walk_animation_id)
    {
        if (astr_ent.flying)
        {
            anim_ent.set_current_animation(astronaut_fly_animation_id);
        }
        else if (!astr_ent.moving)
        {
            anim_ent.set_current_animation(astronaut_idle_animation_id);
        }
    }
    else if (curr_anim_id == astronaut_fly_animation_id)
    {
        if (!astr_ent.flying)
        {
            if (astr_ent.moving)
            {
                anim_ent.set_current_animation(astronaut_walk_animation_id);
            }
            else
            {
                anim_ent.set_current_animation(astronaut_idle_animation_id);
            }
        }
    }
}