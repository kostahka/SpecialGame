#include "Kengine/event/event.hxx"

#include "Kengine/engine.hxx"
#include "event-engine.hxx"
#include "handle-user-event.hxx"
#include "imgui_impl_sdl3.h"

#include <SDL_events.h>
#include <SDL_mouse.h>

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <iostream>

namespace Kengine::event
{
bool poll_events(game* game, SDL_Window* window)
{
    bool      no_quit = true;
    SDL_Event sdl_event;

    while (SDL_PollEvent(&sdl_event))
    {
        game_event event{ event::type::unknown };

        int screen_width;
        int screen_height;
        int pixel_w;
        int pixel_h;

        switch (sdl_event.type)
        {
            case SDL_EVENT_WINDOW_RESIZED:
                SDL_GetWindowSize(window, &screen_width, &screen_height);
                SDL_GetWindowSizeInPixels(window, &pixel_w, &pixel_h);
                glViewport(0, 0, pixel_w, pixel_h);
                engine::instance()->e_game->configuration.screen_width =
                    screen_width;
                engine::instance()->e_game->configuration.screen_height =
                    screen_height;
                engine::instance()->e_game->configuration.pixels_width =
                    pixel_w;
                engine::instance()->e_game->configuration.pixels_height =
                    pixel_h;
                event.g_type = type::window_resize;
                break;
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
            case SDL_EVENT_FINGER_MOTION:
                SDL_GetWindowSize(window, &screen_width, &screen_height);
                event.g_type          = type::touch_move_event;
                event.touch.touch_id  = sdl_event.tfinger.touchId;
                event.touch.finger_id = sdl_event.tfinger.fingerId;
                event.touch.x         = sdl_event.tfinger.x * screen_width;
                event.touch.y         = sdl_event.tfinger.y * screen_height;
                event.touch.pressed   = true;
                break;
            case SDL_EVENT_FINGER_UP:
            case SDL_EVENT_FINGER_DOWN:
                SDL_GetWindowSize(window, &screen_width, &screen_height);
                event.g_type          = type::touch_event;
                event.touch.touch_id  = sdl_event.tfinger.touchId;
                event.touch.finger_id = sdl_event.tfinger.fingerId;
                event.touch.x         = sdl_event.tfinger.x * screen_width;
                event.touch.y         = sdl_event.tfinger.y * screen_height;
                event.touch.pressed   = sdl_event.type == SDL_EVENT_FINGER_DOWN;

                std::cout << "Finger #" << event.touch.finger_id << " "
                          << (event.touch.pressed ? "pressed" : "unpressed")
                          << std::endl;
                if (sdl_event.type == SDL_EVENT_FINGER_DOWN)
                    SDL_WarpMouseInWindow(window,
                                          sdl_event.tfinger.x * screen_width,
                                          sdl_event.tfinger.y * screen_height);
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

        ImGui_ImplSDL3_ProcessEvent(&sdl_event);

        if (event.g_type != type::unknown)
            engine::instance()->e_game->on_event(event);
    }

    SDL_PumpEvents();
    input::keyboard::update();
    input::mouse::update();

    return no_quit;
};
} // namespace Kengine::event
