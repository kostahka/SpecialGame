#include "components/astronaut-component.hxx"

#include "Kengine/imgui/imgui-edit.hxx"
#include "Kengine/scene/scene.hxx"

#include "box2d/b2_fixture.h"

#include "imgui.h"

astronaut_component::astronaut_component()
    : component(name)
    , ground_ray_cast_callback(*this)
{
}

astronaut_component::astronaut_component(Kengine::scene* sc,
                                         entt::entity    cur_entity)
    : component(name)
    , ground_ray_cast_callback(*this)
    , sc(sc)
    , current_entity(cur_entity)
{
}

astronaut_component::astronaut_component(Kengine::scene& sc,
                                         entt::entity    ent,
                                         entt::entity    walk_sound_entity,
                                         entt::entity    fly_sound_entity)
    : component(name)
    , ground_ray_cast_callback(*this)
    , sc(&sc)
    , current_entity(ent)
    , walk_sound_entity(walk_sound_entity)
    , fly_sound_entity(fly_sound_entity)
{
}

astronaut_component::astronaut_component(astronaut_component& other,
                                         Kengine::scene*      sc,
                                         entt::entity         cur_entity,
                                         entt::entity         walk_sound_entity,
                                         entt::entity         fly_sound_entity,
                                         entt::entity hand_anchor_entity,
                                         entt::entity pistol_entity,
                                         entt::entity drill_entity)
    : component(name)
    , ground_ray_cast_callback(*this)
    , sc(sc)
    , current_entity(cur_entity)
    , walk_sound_entity(walk_sound_entity)
    , fly_sound_entity(fly_sound_entity)
    , hand_anchor_entity(hand_anchor_entity)
    , pistol_entity(pistol_entity)
    , drill_entity(drill_entity)
{
    hp          = other.hp;
    current_gun = other.current_gun;
    enemy       = other.enemy;
    move_speed  = other.move_speed;
    fly_speed   = other.fly_speed;
}

astronaut_component::astronaut_component(astronaut_component& other)
    : component(other)
    , ground_ray_cast_callback(*this)
{
    hp                 = other.hp;
    current_gun        = other.current_gun;
    enemy              = other.enemy;
    move_speed         = other.move_speed;
    fly_speed          = other.fly_speed;
    current_entity     = other.current_entity;
    walk_sound_entity  = other.walk_sound_entity;
    fly_sound_entity   = other.fly_sound_entity;
    hand_anchor_entity = other.hand_anchor_entity;
    pistol_entity      = other.pistol_entity;
    drill_entity       = other.drill_entity;
    sc                 = other.sc;
}

astronaut_component::astronaut_component(astronaut_component&& other)
    : component(other)
    , ground_ray_cast_callback(*this)
{
    hp                 = other.hp;
    current_gun        = other.current_gun;
    enemy              = other.enemy;
    move_speed         = other.move_speed;
    fly_speed          = other.fly_speed;
    current_entity     = other.current_entity;
    walk_sound_entity  = other.walk_sound_entity;
    fly_sound_entity   = other.fly_sound_entity;
    hand_anchor_entity = other.hand_anchor_entity;
    pistol_entity      = other.pistol_entity;
    drill_entity       = other.drill_entity;
    sc                 = other.sc;
}

astronaut_component& astronaut_component::operator=(astronaut_component& other)
{
    hp                 = other.hp;
    current_gun        = other.current_gun;
    enemy              = other.enemy;
    move_speed         = other.move_speed;
    fly_speed          = other.fly_speed;
    current_entity     = other.current_entity;
    walk_sound_entity  = other.walk_sound_entity;
    fly_sound_entity   = other.fly_sound_entity;
    hand_anchor_entity = other.hand_anchor_entity;
    pistol_entity      = other.pistol_entity;
    drill_entity       = other.drill_entity;
    sc                 = other.sc;

    return *this;
}

astronaut_component& astronaut_component::operator=(astronaut_component&& other)
{
    hp                 = other.hp;
    current_gun        = other.current_gun;
    enemy              = other.enemy;
    move_speed         = other.move_speed;
    fly_speed          = other.fly_speed;
    current_entity     = other.current_entity;
    walk_sound_entity  = other.walk_sound_entity;
    fly_sound_entity   = other.fly_sound_entity;
    hand_anchor_entity = other.hand_anchor_entity;
    pistol_entity      = other.pistol_entity;
    drill_entity       = other.drill_entity;
    sc                 = other.sc;

    return *this;
}

std::size_t astronaut_component::serialize(std::ostream& os) const
{
    std::size_t size = 0;

    size += Kengine::serialization::write(os, hp);
    size += Kengine::serialization::write(os, current_gun);
    size += Kengine::serialization::write(os, enemy);
    size += Kengine::serialization::write(os, walk_sound_entity);
    size += Kengine::serialization::write(os, fly_sound_entity);
    size += Kengine::serialization::write(os, move_speed);
    size += Kengine::serialization::write(os, fly_speed);
    size += Kengine::serialization::write(os, current_entity);
    size += Kengine::serialization::write(os, hand_anchor_entity);
    size += Kengine::serialization::write(os, pistol_entity);
    size += Kengine::serialization::write(os, drill_entity);

    return size;
}

std::size_t astronaut_component::deserialize(std::istream& is)
{
    std::size_t size = 0;

    size += Kengine::serialization::read(is, hp);
    size += Kengine::serialization::read(is, current_gun);
    size += Kengine::serialization::read(is, enemy);
    size += Kengine::serialization::read(is, walk_sound_entity);
    size += Kengine::serialization::read(is, fly_sound_entity);
    size += Kengine::serialization::read(is, move_speed);
    size += Kengine::serialization::read(is, fly_speed);
    size += Kengine::serialization::read(is, current_entity);
    size += Kengine::serialization::read(is, hand_anchor_entity);
    size += Kengine::serialization::read(is, pistol_entity);
    size += Kengine::serialization::read(is, drill_entity);

    return size;
}

std::size_t astronaut_component::serialize_size() const
{
    std::size_t size = 0;

    size += Kengine::serialization::size(hp);
    size += Kengine::serialization::size(current_gun);
    size += Kengine::serialization::size(enemy);
    size += Kengine::serialization::size(walk_sound_entity);
    size += Kengine::serialization::size(fly_sound_entity);
    size += Kengine::serialization::size(move_speed);
    size += Kengine::serialization::size(fly_speed);
    size += Kengine::serialization::size(current_entity);
    size += Kengine::serialization::size(hand_anchor_entity);
    size += Kengine::serialization::size(pistol_entity);
    size += Kengine::serialization::size(drill_entity);

    return size;
}

void astronaut_component::shoot() {}

bool astronaut_component::imgui_editable_render()
{
    bool edited = false;

    ImGui::PushID(this);

    edited = edited || Kengine::imgui::edit_entity("Walk sound entity",
                                                   walk_sound_entity);
    edited = edited ||
             Kengine::imgui::edit_entity("Fly sound entity", fly_sound_entity);
    edited = edited || Kengine::imgui::edit_entity("Hand anchor entity",
                                                   hand_anchor_entity);
    edited =
        edited || Kengine::imgui::edit_entity("Pistol entity", pistol_entity);
    edited =
        edited || Kengine::imgui::edit_entity("Drill entity", drill_entity);

    edited = edited || ImGui::DragFloat("Move speed", &move_speed, 0.01f);
    edited = edited || ImGui::DragFloat("Fly speed", &fly_speed, 0.01f);
    edited = edited || ImGui::DragFloat("HP", &hp, 1.0f);
    edited = edited || ImGui::Checkbox("Is enemy", &enemy);

    ImGui::PopID();

    return edited;
}

template <>
void Kengine::archive_input::operator()(astronaut_component& value)
{
    total_size += serialization::read(is, value);
    value.initialize(&sc);
}

template <>
void Kengine::archive_continuous_input::operator()(astronaut_component& value)
{
    total_size += serialization::read(is, value);
    value.initialize(&sc, loader.map(value.get_current_entity()));
    value.walk_sound_entity  = loader.map(value.walk_sound_entity);
    value.fly_sound_entity   = loader.map(value.fly_sound_entity);
    value.hand_anchor_entity = loader.map(value.hand_anchor_entity);
    value.pistol_entity      = loader.map(value.pistol_entity);
    value.drill_entity       = loader.map(value.drill_entity);
}

Kengine::component_info astronaut_component::info{
    [](Kengine::scene& sc, entt::entity ent)
    { return sc.registry.any_of<astronaut_component>(ent); },

    [](Kengine::scene& sc, entt::entity ent) {
        return static_cast<component*>(
            &sc.registry.get<astronaut_component>(ent));
    },

    [](entt::snapshot& snapshot, Kengine::archive_output& output)
    { snapshot.get<astronaut_component>(output); },

    [](entt::snapshot& snapshot, Kengine::archive_size& output)
    { snapshot.get<astronaut_component>(output); },

    [](entt::snapshot_loader& snapshot, Kengine::archive_input& input)
    { snapshot.get<astronaut_component>(input); },

    [](entt::continuous_loader&           snapshot,
       Kengine::archive_continuous_input& input)
    { snapshot.get<astronaut_component>(input); },

    [](Kengine::scene& sc, entt::entity ent)
    { sc.registry.erase<astronaut_component>(ent); },

    [](Kengine::scene& sc, entt::entity ent)
    { sc.registry.emplace<astronaut_component>(ent, &sc, ent); },

    [](Kengine::scene& sc, entt::entity ent)
    { sc.registry.patch<astronaut_component>(ent); },

    [](Kengine::scene&                                sc,
       entt::entity                                   ent,
       component*                                     other,
       std::unordered_map<entt::entity, entt::entity> map)
    {
        auto            other_astr = static_cast<astronaut_component*>(other);

        auto            new_walk_sound_entity_it = map.find(other_astr->walk_sound_entity);
        auto            new_fly_sound_entity_it = map.find(other_astr->fly_sound_entity);
        auto            new_hand_anchor_entity_it =
            map.find(other_astr->hand_anchor_entity);
        auto            new_pistol_entity_it = map.find(other_astr->pistol_entity);
        auto            new_drill_entity_it = map.find(other_astr->drill_entity);

        auto            new_walk_sound_entity = new_walk_sound_entity_it != map.end()
                                                    ? new_walk_sound_entity_it->second
                                                    : entt::null;
        auto            new_fly_sound_entity = new_fly_sound_entity_it != map.end()
                                                   ? new_fly_sound_entity_it->second
                                                   : entt::null;
        auto            new_hand_anchor_entity = new_hand_anchor_entity_it != map.end()
                                                     ? new_hand_anchor_entity_it->second
                                                     : entt::null;
        auto            new_pistol_entity = new_pistol_entity_it != map.end()
                                                ? new_pistol_entity_it->second
                                                : entt::null;
        auto            new_drill_entity  = new_drill_entity_it != map.end()
                                                ? new_drill_entity_it->second
                                                : entt::null;
        sc.registry.emplace<astronaut_component>(ent,
                                                 *other_astr,
                                                 &sc,
                                                 ent,
                                                 new_walk_sound_entity,
                                                 new_fly_sound_entity,
                                                 new_hand_anchor_entity,
                                                 new_pistol_entity,
                                                 new_drill_entity);
    },
};

float astronaut_component::GroundRayCastCallback::ReportFixture(
    b2Fixture*    fixture,
    const b2Vec2& point,
    const b2Vec2& normal,
    float         fraction)
{
    if (!astro.sc || !astro.sc->registry.valid(astro.current_entity) ||
        !astro.sc->registry.any_of<Kengine::physics_component>(
            astro.current_entity))
    {
        return 0;
    }
    auto& ph_ent = astro.sc->registry.get<Kengine::physics_component>(
        astro.current_entity);
    if (fixture->GetBody() != ph_ent.get())
    {
        astro.on_ground = true;
        return 0;
    }

    return -1;
}

astronaut_component::GroundRayCastCallback::GroundRayCastCallback(
    astronaut_component& astro)
    : astro(astro)
{
}