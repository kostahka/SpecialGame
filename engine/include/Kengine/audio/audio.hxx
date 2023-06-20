#pragma once

#include <string_view>

namespace Kengine::audio
{
class sound_buffer
{
public:
    virtual uint8_t* get_data() const   = 0;
    virtual uint32_t get_length() const = 0;

    virtual ~sound_buffer();
};

class sound_object
{
public:
    virtual void play() = 0;
    virtual void stop() = 0;

    virtual bool get_is_playing() = 0;

    virtual void set_loop(bool)    = 0;
    virtual void set_volume(float) = 0; // set from 0 to 1

    virtual ~sound_object();
};

sound_buffer* create_sound_buffer(std::string_view wav_path);
sound_object* create_sound_object(sound_buffer* s_buff);
} // namespace Kengine::audio