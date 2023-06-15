#include "game-object/game-object.hxx"

#include "game.hxx"

game_object::game_object()
{
    current_game->add_game_object(this);
}

game_object::~game_object()
{
    current_game->remove_game_object(this);
}
