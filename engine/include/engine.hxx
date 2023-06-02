#ifndef ENGINE_H_
#define ENGINE_H_

#include "glm/fwd.hpp"
#include "shader-program.hxx"
#include "vertex-array-object.hxx"

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

enum class event_type
{
    key_pressed,
    key_released,

    mouse_event,

    quit,

    unknown
};

enum class key_name
{
    left,
    right,
    up,
    down,

    unknown
};

enum class mouse_button
{
    left,
    middle,
    right,
    x1,
    x2,

    unknown
};

enum class button_state
{
    pressed,
    released,

    unknown
};

struct mouse_state
{
    bool  is_pressed;
    float x;
    float y;
};

struct mouse_button_event
{
    mouse_button button;
    uint8_t      clicks;
    button_state state;
    int8_t       x;
    int8_t       y;
};

struct event
{
    event_type type;
    union
    {
        key_name           key;
        mouse_button_event mouse;
    };
};

// game configuratiions
struct game_configuration
{
    // window sizes
    int screen_width;
    int screen_height;
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

    virtual void clear_color(color) = 0;
    virtual void swap_buffers()     = 0;

    engine_configuration configuration{
        std::chrono::milliseconds{ 1000 / 60 },
        std::chrono::milliseconds{ 1000 / 90 },
    };
};

bool        is_key_pressed(key_name key);
mouse_state get_mouse_state(mouse_button button);

engine* get_engine_instance();

class game
{
public:
    game(std::string name, engine* engine)
        : name(name)
        , game_engine(engine){};

    virtual ~game()              = default;
    virtual void on_start()      = 0;
    virtual void on_event(event) = 0;
    virtual void on_update(
        std::chrono::duration<int, std::milli> delta_time) = 0;
    virtual void on_render(
        std::chrono::duration<int, std::milli> delta_time) const = 0;

    std::string        name        = "";
    engine*            game_engine = nullptr;
    game_configuration configuration{ 640, 480 };

    glm::mat4 projection;
    glm::mat4 view;
};

extern shader_program* sprite_program;

}; // namespace Kengine

extern "C" Kengine::game* create_game(Kengine::engine*);

#endif // ENGINE_H_
