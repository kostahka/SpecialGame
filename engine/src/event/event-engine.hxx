#pragma once

#include "Kengine/engine.hxx"

#include "SDL3/SDL_video.h"

namespace Kengine::event
{
bool poll_events(game* game, SDL_Window* window);
}
