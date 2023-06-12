#include "player.hxx"
#include <iostream>
#include <numbers>

#include "engine.hxx"
#include "input.hxx"

#include "camera.hxx"
#include "game.hxx"
#include "resources.hxx"

constexpr float player_size       = 50;
constexpr float player_density    = 0.1;
constexpr float player_move_speed = 50.0f;
constexpr float player_fly_speed  = 100.0f;

player::player(const Kengine::transform2d& pos)
    : player_sprite(resources::player_texture,
                    { 0, 0, 52, 52 },
                    pos,
                    { player_size, player_size })

{
    player_sprite.set_origin({ 0.5, 0.5 });
    player_sprite.set_angle(0);
    b2BodyDef player_body_def;
    player_body_def.position.Set(pos.x, pos.y);
    player_body_def.type          = b2_dynamicBody;
    player_body_def.fixedRotation = true;

    player_body = current_game->physics_world.CreateBody(&player_body_def);

    b2CircleShape player_shape;
    player_shape.m_radius = player_size / 4;
    player_shape.m_p.Set(0, player_size / 4);
    player_body->CreateFixture(&player_shape, player_density);
    player_shape.m_p.Set(0, -player_size / 4);
    player_body->CreateFixture(&player_shape, player_density);
}
void player::draw() const
{
    player_sprite.draw();
}
void player::update(std::chrono::duration<int, std::milli> delta_time)
{
    b2Vec2      pos = player_body->GetPosition();
    transform2d distance_to_planet =
        current_game->land.get_center() - transform2d(pos.x, pos.y);

    glm::vec2 dir_vector =
        glm::normalize(glm::vec2(distance_to_planet.x, distance_to_planet.y));
    float stand_angle;
    if (dir_vector.y > 0)
        stand_angle = (std::acos(dir_vector.x) / std::numbers::pi + 1) * 180.0f;
    else
        stand_angle = 180.f * (1 - std::acos(dir_vector.x) / std::numbers::pi);

    const float gravity_impulse =
        current_game->land.gravity_force * delta_time.count();
    player_body->ApplyLinearImpulseToCenter(
        b2Vec2(dir_vector.x * gravity_impulse, dir_vector.y * gravity_impulse),
        true);

    const float  delta_impulse = delta_time.count() * player_move_speed;
    const b2Vec2 d_impulse_vec(-delta_impulse * dir_vector.y,
                               delta_impulse * dir_vector.x);
    const float  delta_fly_impulse = delta_time.count() * player_fly_speed;
    const b2Vec2 d_fly_vec(delta_fly_impulse * -dir_vector.x,
                           delta_fly_impulse * -dir_vector.y);
    using namespace Kengine::input;
    if (keyboard::key_pressed(keyboard::key::key_a))
    {
        player_sprite.set_size({ -player_size, player_size });
        player_body->ApplyLinearImpulseToCenter(-d_impulse_vec, true);
    }
    if (keyboard::key_pressed(keyboard::key::key_d))
    {
        player_sprite.set_size({ player_size, player_size });
        player_body->ApplyLinearImpulseToCenter(d_impulse_vec, true);
    }
    if (keyboard::key_pressed(keyboard::key::key_w))
    {
        player_body->ApplyLinearImpulseToCenter(d_fly_vec, true);
    }

    if (mouse::button_pressed(mouse::button::left))
    {
    }

    player_body->SetTransform(player_body->GetPosition(),
                              (stand_angle - 90) / 180 * std::numbers::pi);
    player_sprite.set_angle(stand_angle - 90);
    player_sprite.set_pos({ pos.x, pos.y });
};

transform2d player::get_pos() const
{
    b2Vec2 pos = player_body->GetPosition();
    return { pos.x, pos.y };
};
