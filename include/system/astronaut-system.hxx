#pragma once

#include "Kengine/components/animation-component.hxx"
#include "Kengine/system/system.hxx"

#include "astronaut/gun.hxx"
#include "components/astronaut-component.hxx"

#include <memory>

class astronaut_system : public Kengine::system
{
public:
    static constexpr auto name = "astronaut_system";

    astronaut_system(Kengine::scene&);

    ~astronaut_system();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    void on_start(Kengine::scene&) override;
    void on_render(Kengine::scene&, int delta_ms) override;
    void on_update(Kengine::scene&, int delta_ms) override;

    Kengine::scene& sc;

private:
    std::shared_ptr<Kengine::scene> bullet_scene = nullptr;

    void process_animation(astronaut_component&          astr_ent,
                           Kengine::animation_component& anim_ent);

    void bind_damage_interface(entt::registry& reg, entt::entity ent);
};
