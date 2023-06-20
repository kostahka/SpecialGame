#include "Kengine/event/event.hxx"
#include "event-engine.hxx"

#include "handle-user-event.hxx"

#include "Kengine/engine.hxx"
#include "imgui_impl_sdl3.h"
#include <SDL_events.h>

namespace Kengine::event
{
bool poll_events(game* game)
{
    bool      no_quit = true;
    SDL_Event sdl_event;

    while (SDL_PollEvent(&sdl_event))
    {
        ImGui_ImplSDL3_ProcessEvent(&sdl_event);

        game_event event;
        switch (sdl_event.type)
        {
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                event.g_type       = type::keyboard_event;
                event.keyboard.key = static_cast<input::keyboard::key>(
                    sdl_event.key.keysym.scancode);
                event.keyboard.pressed = sdl_event.type == SDL_EVENT_KEY_DOWN;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (ImGui::GetIO().WantCaptureMouse)
                {
                    event.g_type = type::unknown;
                    break;
                }
                event.g_type       = type::mouse_button_event;
                event.mouse.button = static_cast<input::mouse::button>(
                    SDL_BUTTON(sdl_event.button.button));
                event.mouse.pressed =
                    sdl_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
                event.mouse.clicks = sdl_event.button.clicks;
                event.mouse.x      = sdl_event.button.x;
                event.mouse.y      = sdl_event.button.y;
                break;
            case SDL_EVENT_QUIT:
                event.g_type = type::quit;
                no_quit      = false;
                break;
            case SDL_EVENT_USER:
                event.g_type = type::unknown;
                handle_user_event(sdl_event.user);
            default:
                event.g_type = type::unknown;
        }

        if (event.g_type != type::unknown)
            engine::instance()->e_game->on_event(event);
    }
    input::keyboard::update();
    input::mouse::update();

    return no_quit;
};
} // namespace Kengine::event
