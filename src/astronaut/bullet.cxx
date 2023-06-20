#include "astronaut/bullet.hxx"
#include <iostream>

#include "physics/collision_interface.hxx"
#include "physics/physics.hxx"
#include "render/resources.hxx"

constexpr float bullet_size     = 6.f;
constexpr float bullet_density  = 0.1f;
constexpr float bullet_velocity = 100.f;

constexpr int   bullet_damage        = 20;
constexpr float bullet_ground_damage = 0.01f;
constexpr float bullet_damage_radius = 3.f;

constexpr int bullet_life_time = 3000;

bullet::bullet(const Kengine::transform2d& pos, float angle)
    : bullet_sprite(resources::special_game_texture,
                    { 5 * 64, 2 * 64, 64, 64 },
                    pos,
                    { bullet_size, bullet_size },
                    true)
    , life_time(bullet_life_time)
    , bullet_angle(angle)
{
    bullet_sprite.set_origin({ 0.5, 0.5 });
    bullet_sprite.set_angle(angle);

    b2BodyDef body_def;
    body_def.position.Set(pos.x, pos.y);
    body_def.angle         = angle;
    body_def.type          = b2_dynamicBody;
    body_def.fixedRotation = true;
    // body_def.bullet        = true;
    bullet_body = physics::physics_world.CreateBody(&body_def);

    b2PolygonShape body_shape;
    body_shape.SetAsBox(bullet_size / 3, bullet_size / 8);
    bullet_body->CreateFixture(&body_shape, bullet_density);
}

void bullet::update(std::chrono::duration<int, std::milli> delta_time)
{
    bullet_body->SetLinearVelocity(
        { std::cos(bullet_angle) * bullet_velocity,
          std::sin(bullet_angle) * bullet_velocity });

    b2Vec2 bullet_pos = bullet_body->GetPosition();

    bullet_sprite.set_pos({ bullet_pos.x, bullet_pos.y });
    bullet_sprite.set_angle(bullet_angle);

    for (b2ContactEdge* c = bullet_body->GetContactList(); c; c = c->next)
    {
        if (c->contact->IsTouching())
        {
            auto p = c->other->GetUserData().pointer;
            if (p)
            {
                auto* col_interface = reinterpret_cast<collision_interface*>(p);
                col_interface->Hurt(bullet_damage);
                col_interface->Hurt(bullet_damage_radius,
                                    bullet_ground_damage,
                                    { bullet_pos.x, bullet_pos.y });
            }
            destroy();
            return;
        }
    }

    life_time -= delta_time.count();
    if (life_time < 0)
        destroy();
}

void bullet::render(std::chrono::duration<int, std::milli> delta_time)
{
    bullet_sprite.draw();
}
bullet::~bullet()
{
    physics::physics_world.DestroyBody(bullet_body);
    std::cout << "Deleting bullet" << std::endl;
}
void bullet::destroy()
{
    game_object::destroy();
    std::cout << "Destroy bullet" << std::endl;
}
