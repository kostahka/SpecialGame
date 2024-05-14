#pragma once

#include "Kengine/system/system.hxx"
#include "astronaut/gun.hxx"

class player_system : public Kengine::system
{
public:
    static constexpr auto name = "player_system";

    player_system(Kengine::scene&);
    ~player_system();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    void on_start(Kengine::scene&) override;
    void on_event(Kengine::scene&, const Kengine::event::game_event&) override;
    void on_render(Kengine::scene&, int delta_ms) override;
    void on_update(Kengine::scene&, int delta_ms) override;

    Kengine::scene& sc;

    inline int get_players_count() { return players_count; }

private:
    int  players_count = 1;
    void select_player_gun(gun_type gun);
};
