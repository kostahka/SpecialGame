#include "engine.hxx"

#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <ratio>
#include <string_view>
#include <syncstream>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include "engine-resources.hxx"
#include "event-engine.hxx"
#include "event.hxx"
#include "file-last-modify-listener.hxx"
#include "handle-file-modify.hxx"
#include "handle-user-event.hxx"
#include "opengl-error.hxx"

namespace Kengine
{

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

        window = SDL_CreateWindow("Engine init", 600, 400, SDL_WINDOW_OPENGL);

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
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        e_resources::init();

        IMGUI_CHECKVERSION();

        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init("#version 300 es");
        return "good";
    };

    std::string_view uninitialize() override
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

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

        SDL_SetWindowTitle(window, e_game->name.c_str());
        SDL_SetWindowSize(window,
                          e_game->configuration.screen_width,
                          e_game->configuration.screen_height);

        glViewport(0,
                   0,
                   e_game->configuration.screen_width,
                   e_game->configuration.screen_height);

        start_files_watch();

        bool continue_loop = true;

        e_game->on_start();

        update_time = game_clock.now();
        render_time = game_clock.now();

        while (continue_loop)
        {
            handle_file_modify_listeners();

            continue_loop = event::poll_events(e_game);

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

    void set_cursor_visible(bool visible) override
    {
        int failure;
        if (visible)
        {
            failure = SDL_ShowCursor();
            ImGui::GetIO().ConfigFlags ^= ImGuiConfigFlags_NoMouseCursorChange;
        }
        else
        {
            failure = SDL_HideCursor();
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        }

        if (failure)
        {
            std::cerr << "Error to set cursor visibility. Error: "
                      << SDL_GetError() << std::endl;
        }
    };

    void clear_color(color col) override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl_get_error(__LINE__, __FILE__);
        glClearColor(col.r, col.g, col.b, col.a);
        gl_get_error(__LINE__, __FILE__);
    };

    void draw_imgui() override
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowDemoWindow();
        e_game->on_imgui_render();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    SDL_Window*   window  = nullptr;
    SDL_GLContext context = nullptr;

    // Time from init SDL in miliseconds
    std::chrono::high_resolution_clock::time_point update_time;
    std::chrono::high_resolution_clock::time_point render_time;

    std::chrono::high_resolution_clock game_clock;
};

engine* engine_impl::instance = nullptr;

engine::~engine() = default;

#ifdef ENGINE_DEV
void reload_game(void* data)
{
    auto engine_instance =
        reinterpret_cast<engine_impl*>(engine_impl::instance);
    engine_instance->reload_e_game();
};
#endif

engine* engine::instance()
{
    if (engine_impl::instance == nullptr)
        engine_impl::instance = new engine_impl();

    return engine_impl::instance;
    ;
}
}; // namespace Kengine

#ifdef ENGINE_DEV
int main()
{
    using namespace Kengine;
    engine* engine = engine::instance();

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
