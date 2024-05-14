#pragma once

#include "Kengine/system/system.hxx"
#include "astronaut/gun.hxx"

#include "entt/entt.hpp"

#include <memory>

class landscape_system;

class enemy_spawner_system : public Kengine::system
{
public:
    static constexpr auto name = "enemy_spawner_system";

    enemy_spawner_system(Kengine::scene&, landscape_system& land_system);
    ~enemy_spawner_system();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    void on_start(Kengine::scene&) override;
    void on_event(Kengine::scene&, const Kengine::event::game_event&) override;
    void on_render(Kengine::scene&, int delta_ms) override;
    void on_update(Kengine::scene&, int delta_ms) override;

    Kengine::scene& sc;

private:
    std::shared_ptr<Kengine::scene> enemy_sc = nullptr;

    void on_enemy_destroy(entt::registry& reg, entt::entity ent);

    static int   get_random_wave_time();
    static int   get_random_spawn_time();
    static float get_random_spawn_angle();
    static int   get_random_enemies_count();

    int killed_enemies = 0;

    bool wave_active              = false;
    int  wave_time                = 0;
    int  total_wave_time          = 0;
    int  wave_enemies_count       = 0;
    int  wave_enemies_left_count  = 0;
    int  total_wave_enemies_count = 0;
    int  spawn_time               = 0;

    landscape_system& land_system;
};
