#pragma once

#include "Kengine/system/system.hxx"

class game_gui_system : public Kengine::system
{
public:
    static constexpr auto name = "game_gui_system";

    game_gui_system(Kengine::scene&);
    ~game_gui_system();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    void on_start(Kengine::scene&) override;
    void on_event(Kengine::scene&, const Kengine::event::game_event&) override;
    void on_render(Kengine::scene&, int delta_ms) override;
    void on_update(Kengine::scene&, int delta_ms) override;

    Kengine::scene& sc;
};
