#pragma once

#include "Kengine/transform3d.hxx"

#include "Kengine/render/sprite.hxx"
#include "game-object/game-object.hxx"

class bullet : public game_object
{
public:
    bullet(const Kengine::transform2d& pos, float angle);

    void update(std::chrono::duration<int, std::milli> delta_time) override;
    void render(std::chrono::duration<int, std::milli> delta_time) override;

    void destroy() override;

    ~bullet() override;

private:
    b2Body*         bullet_body;
    Kengine::sprite bullet_sprite;

    float bullet_angle;

    int life_time;
};