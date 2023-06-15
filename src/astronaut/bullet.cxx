#include "astronaut/bullet.hxx"

#include "physics/physics.hxx"
#include "render/resources.hxx"

constexpr float bullet_size     = 30.f;
constexpr float bullet_density  = 0.1f;
constexpr float bullet_velocity = 5000.f;

bullet::bullet(const Kengine::transform2d& pos, float angle)
    : bullet_sprite(resources::special_game_texture,
                    { 5 * 64, 2 * 64, 64, 64 },
                    pos,
                    { bullet_size, bullet_size },
                    true)
{
    bullet_sprite.set_origin({ 0.5, 0.5 });
    bullet_sprite.set_angle(angle);

    b2BodyDef body_def;
    body_def.position.Set(pos.x, pos.y);
    body_def.angle = angle;
    body_def.type  = b2_dynamicBody;

    bullet_body = physics::physics_world.CreateBody(&body_def);

    b2PolygonShape body_shape;
    body_shape.SetAsBox(bullet_size / 3, bullet_size / 8);
    bullet_body->CreateFixture(&body_shape, bullet_density);

    bullet_body->SetLinearVelocity({ std::cos(angle) * bullet_velocity,
                                     std::sin(angle) * bullet_velocity });
}

void bullet::on_event(Kengine::event::game_event e) {}

void bullet::input_process() {}

void bullet::update(std::chrono::duration<int, std::milli> delta_time)
{
    b2Vec2 bullet_pos = bullet_body->GetPosition();

    bullet_sprite.set_pos({ bullet_pos.x, bullet_pos.y });
    bullet_sprite.set_angle(bullet_body->GetAngle());
}

void bullet::render(std::chrono::duration<int, std::milli> delta_time)
{
    bullet_sprite.draw();
}

void bullet::destroy() {}
