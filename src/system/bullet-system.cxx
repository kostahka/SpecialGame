#include "system/bullet-system.hxx"

#include "Kengine/components/physics-component.hxx"
#include "Kengine/components/transform-component.hxx"
#include "Kengine/scene/scene.hxx"

#include "components/bullet-component.hxx"
#include "components/damage-interface.hxx"

#include "box2d/b2_contact.h"

bullet_system::bullet_system(Kengine::scene& sc)
    : system(name, Kengine::system_update_type)
    , sc(sc)
{
    sc.registry.on_construct<bullet_component>()
        .connect<&bullet_system::set_bullet_angle>(*this);
    sc.registry.on_construct<Kengine::transform_component>()
        .connect<&bullet_system::set_bullet_angle>(*this);
}

bullet_system::~bullet_system()
{
    sc.registry.on_construct<bullet_component>()
        .disconnect<&bullet_system::set_bullet_angle>(*this);
    sc.registry.on_construct<Kengine::transform_component>()
        .disconnect<&bullet_system::set_bullet_angle>(*this);
}

std::size_t bullet_system::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t bullet_system::deserialize(std::istream& is)
{
    return 0;
}

std::size_t bullet_system::serialize_size() const
{
    return 0;
}

void bullet_system::on_start(Kengine::scene&) {}

void bullet_system::on_event(Kengine::scene&, const Kengine::event::game_event&)
{
}

void bullet_system::on_render(Kengine::scene&, int delta_ms) {}

void bullet_system::on_update(Kengine::scene& sc, int delta_ms)
{
    auto bullet_view = sc.registry.view<Kengine::transform_component,
                                        Kengine::physics_component,
                                        bullet_component>();

    for (auto [ent, trans_ent, ph_ent, bul_ent] : bullet_view.each())
    {
        ph_ent->SetLinearVelocity(
            { std::cos(bul_ent.angle) * bul_ent.velocity,
              std::sin(bul_ent.angle) * bul_ent.velocity });

        Kengine::vec2 bullet_pos = trans_ent.transf.position;

        bul_ent.life_time_less -= delta_ms;
        if (bul_ent.life_time_less < 0)
        {
            sc.registry.destroy(ent);
            continue;
        }

        for (b2ContactEdge* c = ph_ent->GetContactList(); c; c = c->next)
        {
            if (c->contact->IsTouching())
            {
                auto p = c->other->GetUserData().pointer;
                if (p)
                {
                    auto* col_interface =
                        reinterpret_cast<damage_interface*>(p);
                    col_interface->hurt(bul_ent.damage);
                    col_interface->hurt(bul_ent.damage_radius,
                                        bul_ent.ground_damage,
                                        { bullet_pos.x, bullet_pos.y },
                                        gun_type::pistol);
                }
                sc.registry.destroy(ent);
                break;
            }
        }
    }
}

void bullet_system::set_bullet_angle(entt::registry& reg, entt::entity ent)
{
    if (reg.all_of<Kengine::transform_component, bullet_component>(ent))
    {
        auto [trans_ent, bul_ent] =
            reg.get<Kengine::transform_component, bullet_component>(ent);

        trans_ent.transf.angle = bul_ent.angle;
    }
}
