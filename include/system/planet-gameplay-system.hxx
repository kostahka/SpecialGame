#pragma once

#include "Kengine/system/system.hxx"
#include "astronaut/gun.hxx"
#include "system/astronaut-system.hxx"
#include "system/landscape-system.hxx"
#include "system/player-system.hxx"

class planet_gameplay_system : public Kengine::system
{
public:
    static constexpr auto name = "planet_gameplay_system";

    planet_gameplay_system(Kengine::scene&);
    ~planet_gameplay_system();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    void on_start(Kengine::scene&) override;
    void on_event(Kengine::scene&, const Kengine::event::game_event&) override;
    void on_render(Kengine::scene&, int delta_ms) override;
    void on_update(Kengine::scene&, int delta_ms) override;

    Kengine::scene& sc;

private:
    astronaut_system astr_system;
    player_system    pl_system;
    landscape_system land_system;
};
