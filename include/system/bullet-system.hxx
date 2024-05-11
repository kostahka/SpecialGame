#pragma once

#include "Kengine/system/system.hxx"
#include "astronaut/gun.hxx"

#include "entt/entt.hpp"

class bullet_system : public Kengine::system
{
public:
    static constexpr auto name = "bullet_system";

    bullet_system(Kengine::scene&);
    ~bullet_system();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    void on_start(Kengine::scene&) override;
    void on_event(Kengine::scene&, const Kengine::event::game_event&) override;
    void on_render(Kengine::scene&, int delta_ms) override;
    void on_update(Kengine::scene&, int delta_ms) override;

    Kengine::scene& sc;

private:
    void set_bullet_angle(entt::registry& reg, entt::entity ent);
};
