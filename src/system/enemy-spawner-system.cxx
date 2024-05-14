#include "system/enemy-spawner-system.hxx"

#include "Kengine/components/text-component.hxx"
#include "Kengine/components/transform-component.hxx"
#include "Kengine/scene/scene.hxx"

#include "components/astronaut-component.hxx"
#include "components/enemies-progress-bar-component.hxx"
#include "components/enemy-component.hxx"
#include "components/enemy-wave-text-component.hxx"
#include "components/player-controller.hxx"
#include "components/progress-bar-component.hxx"
#include "game.hxx"
#include "system/landscape-system.hxx"

constexpr int enemy_min_spawn_time = 2000;
constexpr int enemy_max_spawn_time = 8000;

constexpr int wave_min_time = 10000;
constexpr int wave_max_time = 15000;

constexpr int wave_enemy_min_count = 6;
constexpr int wave_enemy_max_count = 20;

constexpr float spawn_min_angle = 3.14f / 9.f;
constexpr float spawn_max_angle = 3.14f / 4.f;


enemy_spawner_system::enemy_spawner_system(Kengine::scene&   sc,
                                           landscape_system& land_system)
    : system(name, Kengine::system_update_type | Kengine::system_render_type)
    , sc(sc)
    , land_system(land_system)
{
    enemy_sc =
        special_game::instance->load_scene(Kengine::hash_string("enemy"));

    sc.registry.on_destroy<enemy_component>()
        .connect<&enemy_spawner_system::on_enemy_destroy>(*this);
}

enemy_spawner_system::~enemy_spawner_system()
{
    sc.registry.on_destroy<enemy_component>()
        .disconnect<&enemy_spawner_system::on_enemy_destroy>(*this);

    enemy_sc = nullptr;
}

std::size_t enemy_spawner_system::serialize(std::ostream& os) const
{
    return 0;
}

std::size_t enemy_spawner_system::deserialize(std::istream& is)
{
    return 0;
}

std::size_t enemy_spawner_system::serialize_size() const
{
    return 0;
}

void enemy_spawner_system::on_start(Kengine::scene& sc)
{
    sc.instansiate(Kengine::hash_string("enemy-waves-gui"));
}

void enemy_spawner_system::on_event(Kengine::scene&,
                                    const Kengine::event::game_event&)
{
}

void enemy_spawner_system::on_render(Kengine::scene&, int delta_ms)
{
    float progress = 0.f;

    std::stringstream text;
    if (wave_active)
    {
        progress = static_cast<float>(wave_enemies_left_count) /
                   total_wave_enemies_count;
        text << "Enemies left: " << wave_enemies_left_count;
    }
    else
    {
        progress = static_cast<float>(wave_time) / total_wave_time;
        text << "Time to next wave:";
    }

    auto en_progress_bar_view =
        sc.registry
            .view<progress_bar_component, enemies_progress_bar_component>();
    for (auto [ent, progress_bar_ent, en_progress_bar_ent] :
         en_progress_bar_view.each())
    {
        progress_bar_ent.progress = progress;
    }

    auto en_wave_text_view =
        sc.registry.view<Kengine::text_component, enemy_wave_text_component>();
    for (auto [ent, text_ent, en_wave_text_ent] : en_wave_text_view.each())
    {
        text_ent.text = reinterpret_cast<const char32_t*>(text.str().c_str());
    }
}

void enemy_spawner_system::on_update(Kengine::scene& sc, int delta_ms)
{
    auto pl_view = sc.registry.view<Kengine::transform_component,
                                    astronaut_component,
                                    player_controller>();

    entt::entity  target_entity            = entt::null;
    Kengine::vec2 target_pos               = {};
    float         target_angle_to_earth    = 0;
    float         target_to_earth_distance = 0;

    for (auto [player_ent, player_trans_ent, player_astr_ent, pl_ent] :
         pl_view.each())
    {
        target_entity            = player_ent;
        target_pos               = player_trans_ent.transf.position;
        target_angle_to_earth    = player_astr_ent.angle_to_earth;
        target_to_earth_distance = player_astr_ent.to_earth_center_distance;
    }
    if (target_entity != entt::null)
    {
        if (wave_active)
        {
            wave_active = wave_enemies_left_count > 0;
            if (wave_enemies_count > 0)
            {
                spawn_time -= delta_ms;
                if (spawn_time < 0)
                {
                    spawn_time = get_random_spawn_time();

                    const float target_angle = target_angle_to_earth;

                    if (enemy_sc)
                    {
                        auto prefab_enemy_view =
                            enemy_sc->registry
                                .view<Kengine::transform_component,
                                      enemy_component>();

                        for (auto [prefab_ent,
                                   prefab_trans_ent,
                                   prefab_enemy_ent] : prefab_enemy_view.each())
                        {
                            prefab_trans_ent.transf.position =
                                land_system.get_spawn_place(
                                    target_angle + get_random_spawn_angle());
                        }

                        sc.instansiate(enemy_sc);

                        for (auto [prefab_ent,
                                   prefab_trans_ent,
                                   prefab_enemy_ent] : prefab_enemy_view.each())
                        {
                            prefab_trans_ent.transf.position =
                                land_system.get_spawn_place(
                                    target_angle - get_random_spawn_angle());
                        }

                        sc.instansiate(enemy_sc);

                        wave_enemies_count -= 2;
                    }
                }
            }
        }
        else
        {
            wave_time -= delta_ms;
            if (wave_time < 0)
            {
                wave_enemies_count       = get_random_enemies_count();
                wave_enemies_left_count  = wave_enemies_count;
                total_wave_enemies_count = wave_enemies_count;
                spawn_time               = get_random_spawn_time();

                wave_active     = true;
                wave_time       = get_random_wave_time();
                total_wave_time = wave_time;
            }
        }
    }
}

void enemy_spawner_system::on_enemy_destroy(entt::registry& reg,
                                            entt::entity    ent)
{
    killed_enemies++;
    wave_enemies_left_count--;
}

int enemy_spawner_system::get_random_wave_time()
{
    return wave_min_time +
           static_cast<int>((static_cast<float>(rand()) / RAND_MAX) *
                            (wave_max_time - wave_min_time));
}

int enemy_spawner_system::get_random_spawn_time()
{
    return enemy_min_spawn_time +
           static_cast<int>((static_cast<float>(rand()) / RAND_MAX) *
                            (enemy_max_spawn_time - enemy_min_spawn_time));
}

float enemy_spawner_system::get_random_spawn_angle()
{
    return spawn_min_angle + (static_cast<float>(rand()) / RAND_MAX) *
                                 (spawn_max_angle - spawn_min_angle);
}

int enemy_spawner_system::get_random_enemies_count()
{
    return wave_enemy_min_count +
           static_cast<int>(
               (static_cast<float>(rand()) / RAND_MAX) *
               (wave_enemy_max_count / 2 - wave_enemy_min_count / 2)) *
               2;
}