#include "astronaut/astronaut.hxx"
#include "astronaut/bullet.hxx"
#include "physics/physics.hxx"
#include "render/resources.hxx"
#include <numbers>

constexpr float astronaut_size       = 10;
constexpr float astronaut_density    = 0.1;
constexpr float astronaut_move_speed = 0.1f;
constexpr float astronaut_fly_speed  = 0.2f;
constexpr float astronaut_friction   = 0.1f;

constexpr float bullet_distance = 10.0f;

astronaut::astronaut(const Kengine::transform2d& pos, bool enemy)
    : astronaut_anim(resources::special_game_texture,
                     pos,
                     { astronaut_size, astronaut_size })
    , physics_object(true)
    , astronaut_body(nullptr)
    , moving(false)
    , flying(false)
    , move_direction(1)
    , hand_sprite(resources::special_game_texture,
                  { enemy ? 64 : 0, 128, 64, 64 },
                  pos,
                  { astronaut_size, astronaut_size },
                  true)
    , pistol_sprite(resources::special_game_texture,
                    { 64 * 3, 128, 64, 64 },
                    pos,
                    { astronaut_size, astronaut_size },
                    true)
    , drill_sprite(resources::special_game_texture,
                   { 64 * 2, 128, 64, 64 },
                   pos,
                   { astronaut_size, astronaut_size },
                   true)
    , current_gun_sprite(&pistol_sprite)
    , gun_angle(0)
    , hp(100)
{
    astronaut_anim.set_origin({ 0.5, 0.5 });
    astronaut_anim.set_angle(0);

    hand_sprite.set_origin({ 0.3, 0.5 });
    pistol_sprite.set_origin({ 0.3, 0.5 });
    drill_sprite.set_origin({ 0.3, 0.5 });

    std::vector<Kengine::irect> idle_anim;
    std::vector<Kengine::irect> run_anim;
    std::vector<Kengine::irect> fly_anim;

    int irect_y;

    if (enemy)
        irect_y = 0;
    else
        irect_y = 64;

    idle_anim.push_back({ 0, irect_y, 64, 64 });

    for (int i = 0; i < 7; i++)
    {
        run_anim.push_back({ 64 * i, irect_y, 64, 64 });
    }

    fly_anim.push_back({ 64 * 9, irect_y, 64, 64 });

    astronaut_anim.add_animation("idle", idle_anim, 50);
    astronaut_anim.add_animation("run", run_anim, 50);
    astronaut_anim.add_animation("fly", fly_anim, 50);

    astronaut_anim.play();

    b2BodyDef player_body_def;
    player_body_def.position.Set(pos.x, pos.y);
    player_body_def.type          = b2_dynamicBody;
    player_body_def.fixedRotation = true;
    player_body_def.linearDamping = 0.5;
    player_body_def.userData.pointer =
        reinterpret_cast<uintptr_t>(static_cast<collision_interface*>(this));

    astronaut_body = physics::physics_world.CreateBody(&player_body_def);

    b2CircleShape astronaut_shape;
    astronaut_shape.m_radius = (0.9f * astronaut_size) / 4;
    astronaut_shape.m_p.Set(0, (0.9f * astronaut_size) / 4);
    astronaut_body->CreateFixture(&astronaut_shape, astronaut_density);
    astronaut_shape.m_p.Set(0, -(0.9f * astronaut_size) / 4);
    legs_fixture =
        astronaut_body->CreateFixture(&astronaut_shape, astronaut_density);
    legs_fixture->SetFriction(1);
}

void astronaut::move(int direction)
{
    legs_fixture->SetFriction(1);
    move_direction = direction;
    moving         = true;
}

void astronaut::fly()
{
    flying = true;
}

void astronaut::update(std::chrono::duration<int, std::milli> delta_time)
{
    astronaut_anim.set_current_animation("idle");

    if (moving)
    {
        const float  delta_impulse = delta_time.count() * astronaut_move_speed;
        const b2Vec2 d_impulse_vec(-delta_impulse * direction_to_planet.y,
                                   delta_impulse * direction_to_planet.x);

        astronaut_anim.set_current_animation("run");
        astronaut_body->ApplyLinearImpulseToCenter(
            move_direction * d_impulse_vec, true);
        legs_fixture->SetFriction(astronaut_friction);
        moving = false;
    }

    if (flying)
    {
        const float delta_fly_impulse =
            delta_time.count() * astronaut_fly_speed;
        const b2Vec2 d_fly_vec(delta_fly_impulse * -direction_to_planet.x,
                               delta_fly_impulse * -direction_to_planet.y);

        astronaut_anim.set_current_animation("fly");
        astronaut_body->ApplyLinearImpulseToCenter(d_fly_vec, true);
        flying = false;
    }

    float  astronaut_angle = astronaut_body->GetAngle();
    b2Vec2 astronaut_pos   = astronaut_body->GetPosition();

    astronaut_anim.set_angle(astronaut_angle);
    astronaut_anim.set_pos({ astronaut_pos.x, astronaut_pos.y });

    float d_gun_angle;
    int   direction;
    if (std::cos(astronaut_angle - gun_angle) < 0)
    {
        d_gun_angle = gun_angle - std::numbers::pi;
        direction   = -1;
    }
    else
    {
        d_gun_angle = gun_angle;
        direction   = 1;
    }

    if (move_direction != direction)
        astronaut_anim.set_animation_delta_frame(-1);
    else
        astronaut_anim.set_animation_delta_frame(1);

    astronaut_anim.set_size({ direction * astronaut_size, astronaut_size });
    hand_sprite.set_size({ direction * astronaut_size, astronaut_size });
    pistol_sprite.set_size({ direction * astronaut_size, astronaut_size });
    drill_sprite.set_size({ direction * astronaut_size, astronaut_size });
    hand_sprite.set_angle(d_gun_angle);
    pistol_sprite.set_angle(d_gun_angle);
    drill_sprite.set_angle(d_gun_angle);

    hand_sprite.set_pos({ astronaut_pos.x, astronaut_pos.y });
    pistol_sprite.set_pos({ astronaut_pos.x, astronaut_pos.y });
    drill_sprite.set_pos({ astronaut_pos.x, astronaut_pos.y });
}

void astronaut::render(std::chrono::duration<int, std::milli> delta_time)
{
    astronaut_anim.draw(delta_time);
    current_gun_sprite->draw();

    hand_sprite.draw();
}

Kengine::transform2d astronaut::get_pos() const
{
    b2Vec2 pos = astronaut_body->GetPosition();
    return { pos.x, pos.y };
}

b2Body& astronaut::get_physics_body()
{
    return *astronaut_body;
}

void astronaut::aim(float angle)
{
    gun_angle = angle;
}
void astronaut::select_gun(gun g)
{
    current_gun = g;
    switch (g)
    {
        case gun::pistol:
            current_gun_sprite = &pistol_sprite;
            break;
        case gun::drill:
            current_gun_sprite = &drill_sprite;
            break;
    }
};

void astronaut::shoot()
{
    b2Vec2               astronaut_pos = astronaut_body->GetPosition();
    Kengine::transform2d bullet_pos = { std::cos(gun_angle) * bullet_distance,
                                        std::sin(gun_angle) * bullet_distance };
    new bullet(
        { bullet_pos.x + astronaut_pos.x, bullet_pos.y + astronaut_pos.y },
        gun_angle);
}

void astronaut::Hurt(int damage)
{
    hp -= damage;
    if (hp <= 0)
    {
        destroy();
    }
}
int astronaut::get_hp() const
{
    return hp;
}
void astronaut::Hurt(float                       radius,
                     float                       damage,
                     const Kengine::transform2d& pos)
{
}
astronaut::~astronaut()
{
    physics::physics_world.DestroyBody(astronaut_body);
};
