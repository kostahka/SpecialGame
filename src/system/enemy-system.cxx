#include "system/enemy-system.hxx"

#include "Kengine/components/transform-component.hxx"
#include "Kengine/scene/scene.hxx"

#include "components/astronaut-component.hxx"
#include "components/enemy-component.hxx"
#include "components/player-controller.hxx"

enemy_system::enemy_system(Kengine::scene& sc)
    : system(name, Kengine::system_update_type)
    , sc(sc)
{
}

enemy_system::~enemy_system() {}

std::size_t enemy_system::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t enemy_system::deserialize(std::istream& is)
{
    return 0;
}

std::size_t enemy_system::serialize_size() const
{
    return 0;
}

void enemy_system::on_start(Kengine::scene&) {}

void enemy_system::on_event(Kengine::scene&, const Kengine::event::game_event&)
{
}

void enemy_system::on_render(Kengine::scene&, int delta_ms) {}

void enemy_system::on_update(Kengine::scene& sc, int delta_ms)
{
    auto enemy_view = sc.registry.view<Kengine::transform_component,
                                       astronaut_component,
                                       enemy_component>();

    auto pl_view = sc.registry.view<Kengine::transform_component,
                                    astronaut_component,
                                    player_controller>();

    for (auto [ent, trans_ent, astr_ent, enemy_ent] : enemy_view.each())
    {
        entt::entity  target_entity            = entt::null;
        Kengine::vec2 pos                      = trans_ent.transf.position;
        Kengine::vec2 target_pos               = pos;
        Kengine::vec2 delta_vector             = {};
        float         delta_distance           = 500.f;
        float         target_angle_to_earth    = 0;
        float         target_to_earth_distance = 0;

        for (auto [player_ent, player_trans_ent, player_astr_ent, pl_ent] :
             pl_view.each())
        {
            Kengine::vec2 t_pos      = player_trans_ent.transf.position;
            Kengine::vec2 d_vec      = t_pos - pos;
            float         d_distance = glm::length(glm::vec2(d_vec));
            if (d_distance < delta_distance)
            {
                target_entity         = player_ent;
                target_pos            = t_pos;
                delta_vector          = d_vec;
                delta_distance        = d_distance;
                target_angle_to_earth = player_astr_ent.angle_to_earth;
                target_to_earth_distance =
                    player_astr_ent.to_earth_center_distance;
            }
        }
        if (target_entity != entt::null)
        {
            float aim_angle    = std::atan2(delta_vector.y, delta_vector.x);
            astr_ent.gun_angle = aim_angle;

            if (delta_distance > enemy_ent.aim_distance)
            {
                astr_ent.moving = true;
                if (target_angle_to_earth < astr_ent.angle_to_earth)
                {
                    astr_ent.move_direction = 1;
                }
                else
                {
                    astr_ent.move_direction = -1;
                }
            }

            if (target_to_earth_distance > astr_ent.to_earth_center_distance)
                astr_ent.flying = true;

            enemy_ent.reload_time_less -= delta_ms;
            if (enemy_ent.reload_time_less < 0)
            {
                enemy_ent.reload_time_less = enemy_ent.reload_time;
                astr_ent.shoot();
            }
        }
    }
}