#include "astronaut/astronaut.hxx"
#include "astronaut/bullet.hxx"
#include "game.hxx"
#include "physics/collision_interface.hxx"
#include "physics/physics.hxx"
#include "resources.hxx"

#include <iostream>

constexpr float astronaut_size       = 10;
constexpr float astronaut_density    = 0.1f;
constexpr float astronaut_move_speed = 0.1f;
constexpr float astronaut_fly_speed  = 0.2f;
constexpr float astronaut_friction   = 0.1f;

constexpr float bullet_distance    = 10.0f;
constexpr float drill_max_distance = 50.0f;

constexpr float drill_damage              = 0.0005f;
constexpr float drill_damage_radius       = 6;
constexpr float drill_beam_start_distance = 5.f;
constexpr float drill_size                = 3.f;

void drill_beam_program_use(const glm::mat4& model)
{
    resources::drill_beam_program->use();
    resources::drill_beam_program->set_uniform_matrix4fv(
        "projection", engine::instance()->e_game->projection);
    resources::drill_beam_program->set_uniform_matrix4fv(
        "view", engine::instance()->e_game->view);
    resources::drill_beam_program->set_uniform_matrix4fv("model", model);
    float time = static_cast<float>(
        current_game->game_engine->get_time().count() % 10000);
    resources::drill_beam_program->set_uniform1f("time", time);
}

astronaut::astronaut(const Kengine::transform2d& pos, bool enemy)
    : astronaut_anim(resources::special_game_texture,
                     pos,
                     { astronaut_size, astronaut_size })
    , physics_object(true)
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
    , drill_beam_sprite(resources::special_game_texture,
                        { 64 * 4, 64 * 2, 64, 64 },
                        pos,
                        { drill_max_distance, drill_size },
                        true)
    , current_gun_sprite(&pistol_sprite)
    , gun_angle(0)
    , hp(100)
    , current_gun(gun_type::pistol)
    , enemy(enemy)
    , on_ground(false)
    , ground_ray_cast_callback(this)
    , drill_ray_cast_callback(this)
    , drilling(false)
{
    astronaut_anim.set_origin({ 0.5, 0.5 });
    astronaut_anim.set_angle(0);

    hand_sprite.set_origin({ 0.3f, 0.5f });
    pistol_sprite.set_origin({ 0.3f, 0.5f });
    drill_sprite.set_origin({ 0.3f, 0.5f });
    drill_beam_sprite.set_origin({ 0.f, 0.5f });
    drill_beam_sprite.set_program_use_func(&drill_beam_program_use);

    std::vector<Kengine::irect> idle_anim;
    std::vector<Kengine::irect> run_anim;
    std::vector<Kengine::irect> fly_anim;

    int irect_y;

    if (enemy)
        irect_y = 0;
    else
        irect_y = 64;

    idle_anim.push_back({ 0, irect_y, 64, 64 });

    run_anim.reserve(7);
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

    shooting_sound =
        Kengine::audio::create_sound_object(resources::shooting_sound_buffer);
    hurt_sound =
        Kengine::audio::create_sound_object(resources::hurt_sound_buffer);
    fly_sound =
        Kengine::audio::create_sound_object(resources::fly_sound_buffer);
    walking_sound =
        Kengine::audio::create_sound_object(resources::walking_sound_buffer);
    drill_start_sound = Kengine::audio::create_sound_object(
        resources::drill_start_sound_buffer);
    drill_shooting_sound = Kengine::audio::create_sound_object(
        resources::drill_shooting_sound_buffer);
    drill_shooting_sound->set_loop(true);
    drill_shooting_sound->set_volume(0.3f);
    ground_drilling_sound = Kengine::audio::create_sound_object(
        resources::ground_drilling_sound_buffer);
    ground_drilling_sound->set_loop(true);

    // fly_sound->set_volume(0.3f);

    d_lines = Kengine::create_primitive_render(Kengine::primitive_type::lines);
    d_lines->create();
}

void astronaut::move(float direction)
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
    float  astronaut_angle = astronaut_body->GetAngle();
    b2Vec2 astronaut_pos   = astronaut_body->GetPosition();
    float  down_angle      = astronaut_angle - 3.14f / 2.f;

    astronaut_anim.set_current_animation("idle");

    on_ground = false;
    physics::physics_world.RayCast(
        &ground_ray_cast_callback,
        astronaut_pos,
        { astronaut_pos.x + std::cos(down_angle) * astronaut_size / 1.75f,
          astronaut_pos.y + std::sin(down_angle) * astronaut_size / 1.75f });

    if (moving && on_ground)
    {
        if (!walking_sound->get_is_playing())
            walking_sound->play();
    }
    else
    {
        walking_sound->stop();
    }

    if (moving)
    {
        const float delta_impulse =
            static_cast<float>(delta_time.count()) * astronaut_move_speed;
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
        if (!fly_sound->get_is_playing())
            fly_sound->play();
    }
    else
    {
        fly_sound->stop();
    }

    if (flying)
    {
        const float delta_fly_impulse =
            static_cast<float>(delta_time.count()) * astronaut_fly_speed;
        const b2Vec2 d_fly_vec(delta_fly_impulse * -direction_to_planet.x,
                               delta_fly_impulse * -direction_to_planet.y);

        astronaut_anim.set_current_animation("fly");
        astronaut_body->ApplyLinearImpulseToCenter(d_fly_vec, true);
        flying = false;
    }

    astronaut_anim.set_angle(astronaut_angle);
    astronaut_anim.set_pos({ astronaut_pos.x, astronaut_pos.y });

    float d_gun_angle;
    int   direction;
    if (std::cos(astronaut_angle - gun_angle) < 0)
    {
        d_gun_angle = gun_angle - 3.14f;
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

    astronaut_anim.set_size(
        { static_cast<float>(direction) * astronaut_size, astronaut_size });
    hand_sprite.set_size(
        { static_cast<float>(direction) * astronaut_size, astronaut_size });
    pistol_sprite.set_size(
        { static_cast<float>(direction) * astronaut_size, astronaut_size });
    drill_sprite.set_size(
        { static_cast<float>(direction) * astronaut_size, astronaut_size });
    hand_sprite.set_angle(d_gun_angle);
    pistol_sprite.set_angle(d_gun_angle);
    drill_sprite.set_angle(d_gun_angle);

    hand_sprite.set_pos({ astronaut_pos.x, astronaut_pos.y });
    pistol_sprite.set_pos({ astronaut_pos.x, astronaut_pos.y });
    drill_sprite.set_pos({ astronaut_pos.x, astronaut_pos.y });

    drill_beam_sprite.set_pos(
        { astronaut_pos.x + drill_beam_start_distance * std::cos(gun_angle),
          astronaut_pos.y + drill_beam_start_distance * std::sin(gun_angle) });
    drill_beam_sprite.set_angle(gun_angle);
    if (drilling)
    {
        drill_beam_sprite.set_size({ drill_max_distance, drill_size });

        drill_ray_cast_callback.drill_collision_info = 0;
        physics::physics_world.RayCast(
            &drill_ray_cast_callback,
            astronaut_pos,
            { astronaut_pos.x + std::cos(gun_angle) * drill_max_distance,
              astronaut_pos.y + std::sin(gun_angle) * drill_max_distance });

        if (drill_ray_cast_callback.drill_collision_info)
        {
            auto collision_object = reinterpret_cast<collision_interface*>(
                drill_ray_cast_callback.drill_collision_info);
            if (collision_object->hurt(
                    drill_damage_radius,
                    drill_damage * delta_time.count(),
                    { drill_ray_cast_callback.drill_collision_point.x,
                      drill_ray_cast_callback.drill_collision_point.y },
                    gun_type::drill))
            {
                if (!ground_drilling_sound->get_is_playing())
                {
                    ground_drilling_sound->play();
                }
            }

            collision_object->hurt((int)drill_damage * delta_time.count());

            drill_beam_sprite.set_size(
                { drill_ray_cast_callback.drill_distance -
                      drill_beam_start_distance,
                  drill_size });
        }
        else
        {
            ground_drilling_sound->stop();
        }
    }
}

void astronaut::render(std::chrono::duration<int, std::milli> delta_time)
{
    if (drilling)
    {
        drill_beam_sprite.draw();
    }

    astronaut_anim.draw(delta_time);
    current_gun_sprite->draw();

    hand_sprite.draw();

    float  angle = astronaut_body->GetAngle() - 3.14f / 2;
    b2Vec2 pos   = astronaut_body->GetPosition();

    if (current_game->debug_draw)
    {
        d_lines->vertex({ pos.x, pos.y, 10 }, { 0, 100, 0, 0 });
        d_lines->vertex({ pos.x + std::cos(angle) * astronaut_size / 1.75f,
                          pos.y + std::sin(angle) * astronaut_size / 1.75f,
                          10 },
                        { 0.f, 0.7f, 0.f, 1.0f });
        d_lines->draw();
    }
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
void astronaut::select_gun(gun_type g)
{
    current_gun = g;
    switch (g)
    {
        case gun_type::pistol:
            current_gun_sprite = &pistol_sprite;
            drilling           = false;
            drill_shooting_sound->stop();
            ground_drilling_sound->stop();
            break;
        case gun_type::drill:
            current_gun_sprite = &drill_sprite;
            break;
    }
}

void astronaut::shoot()
{
    if (current_gun == gun_type::pistol)
    {
        b2Vec2               astronaut_pos = astronaut_body->GetPosition();
        Kengine::transform2d bullet_pos    = {
            std::cos(gun_angle) * bullet_distance,
            std::sin(gun_angle) * bullet_distance
        };
        new bullet(
            { bullet_pos.x + astronaut_pos.x, bullet_pos.y + astronaut_pos.y },
            gun_angle);

        shooting_sound->play();
    }
    else
    {
        drilling = !drilling;
        if (drilling)
        {
            drill_start_sound->play();
            drill_shooting_sound->play();
        }
        else
        {
            drill_shooting_sound->stop();
            ground_drilling_sound->stop();
        }
    }
}

void astronaut::hurt(int damage)
{
    hurt_sound->play();
    hp -= damage;
    if (hp <= 0)
    {
        destroy();
    }
}
int astronaut::get_hp() const
{
    return static_cast<int>(hp);
}
bool astronaut::hurt(float                       radius,
                     float                       damage,
                     const Kengine::transform2d& pos,
                     gun_type                    g)
{
    return false;
}
astronaut::~astronaut()
{
    physics::physics_world.DestroyBody(astronaut_body);
    if (enemy)
        std::cout << "Deleting enemy astronaut" << std::endl;
    else
        std::cout << "Deleting player astronaut" << std::endl;
    delete shooting_sound;
    delete hurt_sound;
    delete fly_sound;
    delete walking_sound;
    delete drill_shooting_sound;
    delete drill_start_sound;
    delete ground_drilling_sound;
    d_lines->destroy();
    delete d_lines;
}
void astronaut::destroy()
{
    game_object::destroy();
    if (enemy)
        std::cout << "Destroy enemy astronaut" << std::endl;
    else
        std::cout << "Destroy player astronaut" << std::endl;
}
float astronaut::get_angle() const
{
    return astronaut_body->GetAngle();
}

float astronaut::GroundRayCastCallback::ReportFixture(b2Fixture*    fixture,
                                                      const b2Vec2& point,
                                                      const b2Vec2& normal,
                                                      float         fraction)
{
    if (fixture->GetBody() == physics::land.get_body())
    {
        astro->on_ground = true;
        return 0;
    }

    return -1;
}

astronaut::GroundRayCastCallback::GroundRayCastCallback(astronaut* astro)
    : astro(astro)
{
}

float astronaut::DrillRayCastCallback::ReportFixture(b2Fixture*    fixture,
                                                     const b2Vec2& point,
                                                     const b2Vec2& normal,
                                                     float         fraction)
{
    if (fixture->GetBody() != astro->astronaut_body)
    {
        b2Vec2 astro_pos = astro->astronaut_body->GetPosition();
        b2Vec2 delta_pos = point - astro_pos;
        float  length    = delta_pos.Normalize();
        drill_distance   = length;

        drill_collision_info  = fixture->GetBody()->GetUserData().pointer;
        drill_collision_point = point;
        return fraction;
    }
    return -1;
}

astronaut::DrillRayCastCallback::DrillRayCastCallback(astronaut* astro)
    : astro(astro)
{
}
