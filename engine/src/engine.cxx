#include "engine.hxx"

#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_oldnames.h>
#include <SDL_stdinc.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <efsw/efsw.hpp>
#include <exception>
#include <filesystem>
#include <iostream>
#include <ratio>
#include <string>
#include <string_view>
#include <syncstream>

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "file-last-modify-listener.hxx"
#include "glm/fwd.hpp"
#include "handle-file-modify.hxx"
#include "handle-user-event.hxx"
#include "user-events.hxx"

namespace Kengine
{

shader_program* sprite_program;

class engine_impl;

void APIENTRY debug_message(GLenum        source,
                            GLenum        type,
                            GLuint        id,
                            GLenum        severity,
                            GLsizei       length,
                            const GLchar* message,
                            const void*   userParam)
{
    std::osyncstream sync_err(std::cerr);
    sync_err.write(message, length);
    sync_err << std::endl;
};

void gl_get_error(int line, const char* file)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "Error at line " << line << ", at file " << file
                  << std::endl;
        switch (error)
        {
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM" << std::endl;
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE" << std::endl;
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPERATION" << std::endl;
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << "GL_OUT_OF_MEMORY" << std::endl;
                break;
            case GL_STACK_UNDERFLOW:
                std::cerr << "GL_STACK_UNDERFLOW" << std::endl;
                break;
            case GL_STACK_OVERFLOW:
                std::cerr << "GL_STACK_OVERFLOW" << std::endl;
                break;
        }
    }
};

bool is_key_pressed(key_name key)
{
    SDL_PumpEvents();

    const Uint8* state = SDL_GetKeyboardState(NULL);
    switch (key)
    {
        case key_name::left:
            return state[SDL_SCANCODE_LEFT];
            break;
        case key_name::right:
            return state[SDL_SCANCODE_RIGHT];
            break;
        case key_name::up:
            return state[SDL_SCANCODE_UP];
            break;
        case key_name::down:
            return state[SDL_SCANCODE_DOWN];
            break;
        default:
            return false;
            break;
    }
};

mouse_state get_mouse_state(mouse_button button)
{
    Uint32      buttons;
    float       x, y;
    mouse_state m_state;

    SDL_PumpEvents();
    buttons = SDL_GetMouseState(&x, &y);

    m_state.x = x;
    m_state.y = y;
    switch (button)
    {
        case mouse_button::left:
            m_state.is_pressed = buttons & SDL_BUTTON_LMASK;
            break;
        case mouse_button::right:
            m_state.is_pressed = buttons & SDL_BUTTON_RMASK;
            break;
        case mouse_button::middle:
            m_state.is_pressed = buttons & SDL_BUTTON_MMASK;
            break;
        case mouse_button::x1:
            m_state.is_pressed = buttons & SDL_BUTTON_X1MASK;
            break;
        case mouse_button::x2:
            m_state.is_pressed = buttons & SDL_BUTTON_X2MASK;
            break;
        default:
            m_state.is_pressed = false;
            break;
    }
    return m_state;
};

#ifdef ENGINE_DEV
void reload_game(void* data);
#endif

class engine_impl : public engine
{
public:
    static engine* instance;

    std::string_view initialize() override
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            std::cerr << "Error to initialize SDL. Error: " << SDL_GetError()
                      << std::endl;
            return "sdl init fail";
        }

        return "good";
    };

    std::string_view uninitialize() override
    {
        if (context)
            SDL_GL_DeleteContext(context);

        if (window)
            SDL_DestroyWindow(window);

        SDL_Quit();

        return "good";
    };

    std::string_view start_game_loop() override
    {
        if (e_game == nullptr)
            return "game not set";

        window = SDL_CreateWindow(e_game->name.c_str(),
                                  e_game->configuration.screen_width,
                                  e_game->configuration.screen_height,
                                  SDL_WINDOW_OPENGL);

        if (window == nullptr)
        {
            std::cerr << "Error to create window. Error: " << SDL_GetError()
                      << std::endl;
            SDL_Quit();
            return "failed to create window";
        }

        int gl_major_version = 3;
        int gl_minor_version = 2;
        int gl_profile       = SDL_GL_CONTEXT_PROFILE_ES;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_version);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_version);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_profile);

        context = SDL_GL_CreateContext(window);
        if (context == nullptr)
        {
            std::cerr << "Failed to create GL context. Error: "
                      << SDL_GetError() << std::endl;
            return "failed to create context";
        }

        if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                                &gl_major_version) ||
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                                &gl_minor_version))
        {
            std::cerr << "Failed to get GL versions. Error: " << SDL_GetError()
                      << std::endl;
            return "failed to get GL versions";
        }

        if (gl_major_version < 3 || gl_minor_version < 2)
        {
            std::cerr << "Open GL context version is low. Minimum required: 3.2"
                      << std::endl;
            return "open gl version is low";
        }

        std::clog << "Open GL version: " << gl_major_version << "."
                  << gl_minor_version << std::endl;

        auto load_gl_func = [](const char* func_name)
        { return reinterpret_cast<void*>(SDL_GL_GetProcAddress(func_name)); };

        if (gladLoadGLES2Loader(load_gl_func) == 0)
        {
            std::cerr << "Failed to initialize glad" << std::endl;
            return "failed to init glad";
        }

        std::string_view platform{ SDL_GetPlatform() };
        if (platform != "Mac OS X")
        {
            glEnable(GL_DEBUG_OUTPUT);
            glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(&debug_message, nullptr);
            glDebugMessageControl(
                GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }

        glEnable(GL_DEPTH_TEST);

        sprite_program = create_shader_program_from_code(R"(
                #version 300 es
                precision mediump float;

                layout (location = 0) in vec3 a_position;
                layout (location = 1) in vec2 a_tex_coord;
                out vec2 v_tex_coord;

                uniform mat4 projection;
                uniform mat4 view;
                uniform mat4 model;

                void main()
                {
                    v_tex_coord = a_tex_coord;
                    gl_Position = projection * view * model * vec4(a_position, 1.0);
                }
)",
                                                         R"(
                #version 300 es
                precision mediump float;

                in vec2 v_tex_coord;
                uniform sampler2D s_texture;

                out vec4 fragColor;

                void main()
                {
                    fragColor = texture2D(s_texture, v_tex_coord);
                }
)");

        start_files_watch();

        bool      continue_loop = true;
        SDL_Event sdl_event;

        e_game->on_start();

        update_time = game_clock.now();
        render_time = game_clock.now();

        while (continue_loop)
        {
            handle_file_modify_listeners();

            while (SDL_PollEvent(&sdl_event))
            {
                event event;
                switch (sdl_event.type)
                {
                    case SDL_EVENT_KEY_DOWN:
                        event.type = event_type::key_pressed;
                        event.key  = get_key_name(sdl_event.key.keysym.sym);
                        break;
                    case SDL_EVENT_KEY_UP:
                        event.type = event_type::key_released;
                        event.key  = get_key_name(sdl_event.key.keysym.sym);
                        break;
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    case SDL_EVENT_MOUSE_BUTTON_UP:
                        event.type = event_type::mouse_event;
                        event.mouse.button =
                            get_mouse_button(sdl_event.button.button);
                        event.mouse.state =
                            get_button_state(sdl_event.button.state);
                        event.mouse.clicks = sdl_event.button.clicks;
                        event.mouse.x      = sdl_event.button.x;
                        event.mouse.y      = sdl_event.button.y;
                        break;
                    case SDL_EVENT_QUIT:
                        event.type    = event_type::quit;
                        continue_loop = false;
                        break;
                    case SDL_EVENT_USER:
                        event.type = event_type::unknown;
                        handle_user_event(sdl_event.user);
                    default:
                        event.type = event_type::unknown;
                }
                e_game->on_event(event);
            }

            auto update_delta_time = game_clock.now() - update_time;
            if (update_delta_time > configuration.update_delta_time)
            {
                e_game->on_update(duration_cast<std::chrono::milliseconds>(
                    update_delta_time));
                update_time = game_clock.now();
            }
            auto render_delta_time = game_clock.now() - render_time;
            if (render_delta_time > configuration.render_delta_time)
            {
                e_game->on_render(duration_cast<std::chrono::milliseconds>(
                    render_delta_time));
                render_time = game_clock.now();
            }
        }

        return "good";
    };

    void set_game(game* e_game) override { this->e_game = e_game; }

    void clear_color(color col) override
    {
        glClearColor(col.r, col.g, col.b, col.a);
        gl_get_error(__LINE__, __FILE__);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl_get_error(__LINE__, __FILE__);
    };

    void swap_buffers() override
    {
        SDL_GL_SwapWindow(window);
        gl_get_error(__LINE__, __FILE__);
    };

#ifdef ENGINE_DEV
    std::string_view dev_initialization(std::string lib_name,
                                        std::string tmp_lib_name) override
    {
        this->lib_name     = lib_name;
        this->tmp_lib_name = tmp_lib_name;
        return "good";
    };

    std::string_view start_dev_game_loop() override
    {
        if (lib_name == "" || tmp_lib_name == "")
            return "no dev init";

        load_e_game();

        auto file_listener = file_last_modify_listener::get_instance();
        file_listener->add_file(lib_name, &reload_game, nullptr);

        auto loop_return_code = start_game_loop();

        return loop_return_code;
    };

    bool reload_e_game()
    {
        bool result = load_e_game();
        if (result)
            e_game->on_start();
        return false;
    };

    bool load_e_game()
    {
        if (e_game)
        {
            delete e_game;
            SDL_UnloadObject(lib_handle);
        }

        using namespace std::filesystem;
        if (exists(tmp_lib_name))
        {
            if (!remove(tmp_lib_name))
            {
                std::cerr << "Failed to remove temp lib [" << tmp_lib_name
                          << "]" << std::endl;
                return false;
            }
        }

        try
        {
            copy_file(lib_name, tmp_lib_name);
        }
        catch (const std::exception* ex)
        {
            std::cerr << "Failed to copy from [" << lib_name << "] to ["
                      << tmp_lib_name << "]" << std::endl;
            return false;
        }

        lib_handle = SDL_LoadObject(tmp_lib_name.c_str());

        if (lib_handle == nullptr)
        {
            std::cerr << "Failed to load lib from [" << tmp_lib_name
                      << "]. Error: " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_FunctionPointer create_game_func_ptr =
            SDL_LoadFunction(lib_handle, "create_game");

        if (create_game_func_ptr == nullptr)
        {
            std::cerr << "Failed to load function [create_game] from ["
                      << tmp_lib_name << "]. Error: " << SDL_GetError()
                      << std::endl;
            return false;
        }

        using create_game_ptr = decltype(&create_game);

        create_game_ptr create_game_func =
            reinterpret_cast<create_game_ptr>(create_game_func_ptr);

        game* new_game = create_game_func(this);

        if (new_game == nullptr)
        {
            std::cerr << "Failed to create game" << std::endl;
            return false;
        }

        e_game = new_game;

        return true;
    };

    std::string lib_name     = "";
    std::string tmp_lib_name = "";
    void*       lib_handle   = nullptr;

#endif

private:
    // Engine
    game*         e_game  = nullptr;
    SDL_Window*   window  = nullptr;
    SDL_GLContext context = nullptr;
    // SDL keys to engine keys
    key_name get_key_name(SDL_Keycode sdl_key_code)
    {
        switch (sdl_key_code)
        {
            case SDLK_LEFT:
                return key_name::left;
            case SDLK_RIGHT:
                return key_name::right;
            case SDLK_UP:
                return key_name::up;
            case SDLK_DOWN:
                return key_name::down;
            default:
                return key_name::unknown;
        }
    }

    mouse_button get_mouse_button(Uint8 sdl_button)
    {
        switch (sdl_button)
        {
            case SDL_BUTTON_LEFT:
                return mouse_button::left;
            case SDL_BUTTON_RIGHT:
                return mouse_button::right;
            case SDL_BUTTON_MIDDLE:
                return mouse_button::middle;
            case SDL_BUTTON_X1:
                return mouse_button::x1;
            case SDL_BUTTON_X2:
                return mouse_button::x2;
            default:
                return mouse_button::unknown;
        }
    }

    button_state get_button_state(Uint8 sdl_state)
    {

        switch (sdl_state)
        {
            case SDL_PRESSED:
                return button_state::pressed;
            case SDL_RELEASED:
                return button_state::released;
            default:
                return button_state::unknown;
        }
    };

    // Time from init SDL in miliseconds
    std::chrono::high_resolution_clock::time_point update_time;
    std::chrono::high_resolution_clock::time_point render_time;

    std::chrono::high_resolution_clock game_clock;
};

engine* engine_impl::instance = nullptr;

engine::~engine() = default;

engine* get_engine_instance()
{
    if (engine_impl::instance == nullptr)
        engine_impl::instance = new engine_impl();

    return engine_impl::instance;
}

#ifdef ENGINE_DEV
void reload_game(void* data)
{
    auto engine_instance =
        reinterpret_cast<engine_impl*>(engine_impl::instance);
    engine_instance->reload_e_game();
};
#endif

}; // namespace Kengine

#ifdef ENGINE_DEV
int main()
{
    using namespace Kengine;
    engine* engine = get_engine_instance();

    if (engine->initialize() != "good")
        return EXIT_FAILURE;

    using namespace std::string_literals;
    std::string game_name = ENGINE_GAME_LIB_NAME;
    std::string lib_name  = SDL_GetPlatform() == "Windows"s
                                ? game_name + ".dll"
                                : "./lib" + game_name + ".so";

    std::string tmp_lib_name = "./temp.dll";

    engine->dev_initialization(lib_name, tmp_lib_name);

    engine->start_dev_game_loop();

    engine->uninitialize();

    return EXIT_SUCCESS;
};
#endif
