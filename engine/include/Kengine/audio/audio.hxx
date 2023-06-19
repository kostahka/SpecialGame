#pragma once

#include <string_view>

namespace Kengine::audio
{
class sound_buffer
{
public:
    virtual void play() = 0;
    virtual void stop() = 0;

    virtual void set_loop(bool)    = 0;
    virtual void set_volume(float) = 0; // set from 0 to 1

    virtual ~sound_buffer();
};

class sound_object
{
public:
    virtual void play() = 0;
    virtual void stop() = 0;

    virtual void set_loop(bool)    = 0;
    virtual void set_volume(float) = 0; // set from 0 to 1
private:
};

sound_buffer* create_sound_buffer(std::string_view wav_path);
} // namespace Kengine::audio