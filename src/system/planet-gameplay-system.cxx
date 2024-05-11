#include "system/planet-gameplay-system.hxx"

#include "Kengine/components/camera-component.hxx"
#include "Kengine/components/transform-component.hxx"
#include "Kengine/scene/scene.hxx"

#include "components/astronaut-component.hxx"
#include "components/player-controller.hxx"

#include <cstdlib>

planet_gameplay_system::planet_gameplay_system(Kengine::scene& sc)
    : system(name)
    , sc(sc)
    , astr_system(sc)
    , pl_system(sc)
    , land_system(sc)
    , bul_system(sc)
    , en_system(sc)
    , en_spawn_system(sc, land_system)
{
}

planet_gameplay_system::~planet_gameplay_system() {}

std::size_t planet_gameplay_system::serialize(std::ostream& os) const
{
    std::size_t size = 0;

    size += Kengine::serialization::write(os, astr_system);
    size += Kengine::serialization::write(os, pl_system);
    size += Kengine::serialization::write(os, land_system);
    size += Kengine::serialization::write(os, bul_system);
    size += Kengine::serialization::write(os, en_system);
    size += Kengine::serialization::write(os, en_spawn_system);

    return size;
}

std::size_t planet_gameplay_system::deserialize(std::istream& is)
{
    std::size_t size = 0;

    size += Kengine::serialization::read(is, astr_system);
    size += Kengine::serialization::read(is, pl_system);
    size += Kengine::serialization::read(is, land_system);
    size += Kengine::serialization::read(is, bul_system);
    size += Kengine::serialization::read(is, en_system);
    size += Kengine::serialization::read(is, en_spawn_system);

    return size;
}

std::size_t planet_gameplay_system::serialize_size() const
{
    std::size_t size = 0;

    size += Kengine::serialization::size(astr_system);
    size += Kengine::serialization::size(pl_system);
    size += Kengine::serialization::size(land_system);
    size += Kengine::serialization::size(bul_system);
    size += Kengine::serialization::size(en_system);
    size += Kengine::serialization::size(en_spawn_system);

    return size;
}

void planet_gameplay_system::on_start(Kengine::scene& sc)
{
    sc.get_world().SetGravity({ 0.0f, 0.0f });

    land_system.on_start(sc);
    astr_system.on_start(sc);
    pl_system.on_start(sc);
    bul_system.on_start(sc);
    en_system.on_start(sc);
    en_spawn_system.on_start(sc);

    sc.instansiate(Kengine::hash_string("player"));

    auto pl_view = sc.registry.view<Kengine::transform_component,
                                    astronaut_component,
                                    player_controller>();

    for (auto [ent, trans_ent, astr_ent, pl_ent] : pl_view.each())
    {
        trans_ent.transf.position =
            land_system.get_spawn_place(rand() / static_cast<float>(RAND_MAX));
        sc.set_camera(ent);
    }
}

void planet_gameplay_system::on_event(Kengine::scene&                   sc,
                                      const Kengine::event::game_event& g_event)
{
    pl_system.on_event(sc, g_event);
}

void planet_gameplay_system::on_render(Kengine::scene& sc, int delta_ms)
{
    land_system.on_render(sc, delta_ms);
}

void planet_gameplay_system::on_update(Kengine::scene& sc, int delta_ms)
{
    auto astr_view = sc.registry.view<Kengine::transform_component,
                                      Kengine::physics_component,
                                      astronaut_component>();
    for (auto [ent, trans_ent, ph_ent, astr_ent] : astr_view.each())
    {
        Kengine::vec2 to_earth_center =
            land_system.get_center() - trans_ent.transf.position;
        Kengine::vec2 norm_to_earth_center = glm::normalize(to_earth_center);
        float         distance_to_planet =
            static_cast<float>(glm::length(glm::vec2(to_earth_center)));

        astr_ent.to_earth_center_norm     = norm_to_earth_center;
        astr_ent.to_earth_center_distance = distance_to_planet;
        astr_ent.angle_to_earth =
            land_system.get_angle_to(trans_ent.transf.position);

        float stand_angle =
            std::atan2(norm_to_earth_center.y, norm_to_earth_center.x);
        trans_ent.transf.angle = (stand_angle + 3.14f / 2.f);

        if (distance_to_planet < 10)
            distance_to_planet = 10;

        b2Vec2 force_to_planet = Kengine::vec2(
            norm_to_earth_center * (land_system.gravity_force * delta_ms));
        ph_ent->ApplyLinearImpulseToCenter(force_to_planet, true);
    }
    pl_system.on_update(sc, delta_ms);
    astr_system.on_update(sc, delta_ms);
    bul_system.on_update(sc, delta_ms);
    en_system.on_update(sc, delta_ms);
    en_spawn_system.on_update(sc, delta_ms);
}