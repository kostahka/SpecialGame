#pragma once

#include "Kengine/transform3d.hxx"

#include "Kengine/render/sprite.hxx"
#include "game-object/game-object.hxx"

class bullet : public game_object
{
public:
    bullet(const Kengine::transform2d& pos, float angle);

    void on_event(Kengine::event::game_event e) override;
    void input_process() override;
    void update(std::chrono::duration<int, std::milli> delta_time) override;
    void render(std::chrono::duration<int, std::milli> delta_time) override;
    void destroy() override;

private:
    b2Body*         bullet_body;
    Kengine::sprite bullet_sprite;
};