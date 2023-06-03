#include "game.hxx"

#include "engine.hxx"
#include "glm/ext/matrix_clip_space.hpp"
#include "sprite.hxx"
#include "texture.hxx"

using namespace Kengine;

std::chrono::high_resolution_clock game_clock;

my_game::my_game(engine* game_engine)
    : game("My game", game_engine)
    , land(this){};

void my_game::on_start()
{
    start_point = game_clock.now();
    projection  = glm::ortho(0.f,
                            static_cast<float>(configuration.screen_width),
                            0.f,
                            static_cast<float>(configuration.screen_height));
    view        = glm::mat4(1);
    land.init();
    player_texture = create_texture("./assets/astronaut.png");
    player         = new sprite(
        player_texture, { 0, 0, 52, 52 }, { 0, 0 }, { 100, 100 }, this);
    player->set_origin({ 0.5, 0.5 });
    player->set_pos({ 100, 100 });
};

float angle = 0;

void my_game::on_event(event e){};

void my_game::on_update(std::chrono::duration<int, std::milli> delta_time)
{
    mouse_state left_mouse_state = get_mouse_state(mouse_button::left);
    if (left_mouse_state.is_pressed)
    {
        float delta_value = delta_time.count() / 1'000.f;
        land.change_ground(left_mouse_state.x,
                           configuration.screen_height - left_mouse_state.y,
                           20.0,
                           delta_value);
    }
    mouse_state right_mouse_state = get_mouse_state(mouse_button::right);
    if (right_mouse_state.is_pressed)
    {
        float delta_value = -delta_time.count() / 1'000.f;
        land.change_ground(left_mouse_state.x,
                           configuration.screen_height - left_mouse_state.y,
                           20.0,
                           delta_value);
    }

    angle += delta_time.count() / 1'00.f;
    player->set_angle(angle);
};

void my_game::on_render(std::chrono::duration<int, std::milli> delta_time) const
{
    game_engine->clear_color({ 0.0, 0.0, 0.0, 0.0 });

    float time = (game_clock.now() - start_point).count() / 1'000'000'000.f;

    land.draw();
    player->draw();

    game_engine->swap_buffers();
};

my_game::~my_game(){};

game* create_game(engine* e)
{
    if (e != nullptr)
        return new my_game(e);

    return nullptr;
};
