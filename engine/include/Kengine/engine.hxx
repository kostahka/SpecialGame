#ifndef ENGINE_H_
#define ENGINE_H_

#include "Kengine/event/event.hxx"
#include "Kengine/io/input.hxx"
#include "Kengine/render/shader-program.hxx"
#include "Kengine/render/vertex-array-object.hxx"

#include <chrono>
#include <cstdint>
#include <ostream>
#include <ratio>
#include <string>
#include <string_view>

namespace Kengine
{

class game;
class engine;

struct rect
{
    float x;
    float y;
    float w;
    float h;
};

// game configuratiions
struct game_configuration
{
    // window sizes
    int screen_width;
    int screen_height;
    int pixels_width;
    int pixels_height;
};

// Engine configuration
struct engine_configuration
{
    // frequency of physics update
    std::chrono::duration<int, std::milli> update_delta_time;
    // frequency of render update
    std::chrono::duration<int, std::milli> render_delta_time;
};

class engine
{
public:
    static engine* instance();

    virtual ~engine();
    virtual std::string_view initialize()    = 0;
    virtual std::string_view uninitialize()  = 0;
    virtual void             set_game(game*) = 0;

#ifdef ENGINE_DEV
    virtual std::string_view dev_initialization(std::string lib_name,
                                                std::string tmp_lib_name) = 0;
    virtual std::string_view start_dev_game_loop()                        = 0;
#endif
    virtual std::string_view start_game_loop() = 0;

    virtual void set_cursor_visible(bool visible) = 0;
    virtual void clear_color(color)               = 0;
    virtual void swap_buffers()                   = 0;
    virtual void draw_imgui()                     = 0;

    virtual std::chrono::duration<int, std::milli> get_time() = 0;

    virtual void quit() = 0;

    engine_configuration configuration{
        std::chrono::milliseconds{ 1000 / 60 },
        std::chrono::milliseconds{ 1000 / 90 },
    };

    game* e_game = nullptr;
};

class game
{
public:
    game(std::string name, engine* engine)
        : name(name)
        , game_engine(engine){};

    virtual ~game()                          = default;
    virtual void on_start()                  = 0;
    virtual void on_event(event::game_event) = 0;
    virtual void on_update(
        std::chrono::duration<int, std::milli> delta_time) = 0;
    virtual void on_render(
        std::chrono::duration<int, std::milli> delta_time) = 0;
    virtual void on_imgui_render()                         = 0;

    std::string        name        = "";
    engine*            game_engine = nullptr;
    game_configuration configuration{ 640, 480 };

    glm::mat4 projection;
    glm::mat4 view;
};

}; // namespace Kengine

extern "C" Kengine::game* create_game(Kengine::engine*);

#endif // ENGINE_H_
