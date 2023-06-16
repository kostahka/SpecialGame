#include "game-object/controller.hxx"
#include "game.hxx"

controller::controller()
{
    current_game->add_controller(this);
}

controller::~controller() {}

void controller::destroy()
{
    destroyable::destroy();
    current_game->destroy_controller(this);
}
