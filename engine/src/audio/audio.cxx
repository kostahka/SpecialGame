#include "audio.hxx"

#include <algorithm>
#include <iostream>
#include <map>
#include <mutex>

#include "Kengine/audio/audio.hxx"

#include "SDL3/SDL_audio.h"

namespace Kengine::audio
{

std::string_view get_sound_format_name(uint16_t format_value)
{
    static const std::map<int, std::string_view> format = {
        { SDL_AUDIO_U8, "AUDIO_U8" },
        { SDL_AUDIO_S8, "AUDIO_S8" },
        { SDL_AUDIO_S16LSB, "AUDIO_S16LSB" },
        { SDL_AUDIO_S16MSB, "AUDIO_S16MSB" },
        { SDL_AUDIO_S32LSB, "AUDIO_S32LSB" },
        { SDL_AUDIO_S32MSB, "AUDIO_S32MSB" },
        { SDL_AUDIO_F32LSB, "AUDIO_F32LSB" },
        { SDL_AUDIO_F32MSB, "AUDIO_F32MSB" },
    };

    auto it = format.find(format_value);
    return it->second;
}

std::size_t get_sound_format_size(uint16_t format_value)
{
    static const std::map<int, std::size_t> format = {
        { SDL_AUDIO_U8, 1 },     { SDL_AUDIO_S8, 1 },
        { SDL_AUDIO_S16LSB, 2 }, { SDL_AUDIO_S16MSB, 2 },
        { SDL_AUDIO_S32LSB, 4 }, { SDL_AUDIO_S32MSB, 4 },
        { SDL_AUDIO_F32LSB, 4 }, { SDL_AUDIO_F32MSB, 4 },
    };

    auto it = format.find(format_value);
    return it->second;
}
class sound_object_impl;

static SDL_AudioSpec     audio_device_spec;
static SDL_AudioDeviceID audio_device_id;

static std::mutex audio_mutex;

static std::vector<sound_object_impl*> sound_buffers;

class sound_buffer_impl : public sound_buffer
{
public:
    sound_buffer_impl(std::string_view path)
        : buffer(nullptr)
        , length(0)

    {
        //        SDL_RWops* file = SDL_RWFromFile(path.data(), "rb");
        //        if (file == nullptr)
        //        {
        //            std::cerr << "Failed to open [" << path
        //                      << "] to read. Error: " << SDL_GetError() <<
        //                      std::endl;
        //        }

        SDL_AudioSpec file_audio_spec;
        if (!SDL_LoadWAV(path.data(), &file_audio_spec, &buffer, &length))
        {
            std::cerr << "Failed to load wav from [" << path
                      << "] . Error: " << SDL_GetError() << std::endl;
            return;
        }

        std::cout << "Audio format for: " << path << '\n'
                  << "-format: "
                  << get_sound_format_name(file_audio_spec.format) << '\n'
                  << "-sample_size: "
                  << get_sound_format_size(file_audio_spec.format) << '\n'
                  << "-channels: "
                  << static_cast<uint32_t>(file_audio_spec.channels) << '\n'
                  << "-frequency: " << file_audio_spec.freq << '\n'
                  << "-length: " << length << '\n'
                  << "-time: "
                  << static_cast<double>(length) /
                         static_cast<double>(
                             (file_audio_spec.channels *
                              static_cast<uint32_t>(file_audio_spec.freq) *
                              get_sound_format_size(file_audio_spec.format)))
                  << "sec" << std::endl;

        if (file_audio_spec.channels != audio_device_spec.channels ||
            file_audio_spec.format != audio_device_spec.format ||
            file_audio_spec.freq != audio_device_spec.freq)
        {
            Uint8* output_bytes;
            int    output_length;

            int convert_status =
                SDL_ConvertAudioSamples(file_audio_spec.format,
                                        file_audio_spec.channels,
                                        file_audio_spec.freq,
                                        buffer,
                                        static_cast<int>(length),
                                        audio_device_spec.format,
                                        audio_device_spec.channels,
                                        audio_device_spec.freq,
                                        &output_bytes,
                                        &output_length);
            if (convert_status != 0)
            {
                std::cerr << "Failed to convert wav audio. Error"
                          << SDL_GetError() << std::endl;
                return;
            }

            SDL_free(buffer);
            buffer = output_bytes;
            length = static_cast<uint32_t>(output_length);
        }
    };

    [[nodiscard]] uint8_t* get_data() const override { return buffer; };
    [[nodiscard]] uint32_t get_length() const override { return length; };

    ~sound_buffer_impl() override
    {
        std::lock_guard<std::mutex> lock(audio_mutex);

        if (buffer)
        {
            SDL_free(buffer);
        }
        buffer = nullptr;
        length = 0;
    };

    uint8_t* buffer;
    uint32_t length;
};

class sound_object_impl : public sound_object
{
public:
    explicit sound_object_impl(sound_buffer* s_buff)
        : current_index(0)
        , is_playing(false)
        , looped(false)
        , volume(SDL_MIX_MAXVOLUME)
    {
        buffer = s_buff->get_data();
        length = s_buff->get_length();
    };

    void play() override
    {
        std::lock_guard<std::mutex> lock(sound_mutex);
        current_index = 0;
        is_playing    = true;
    };
    void stop() override
    {
        std::lock_guard<std::mutex> lock(sound_mutex);
        is_playing = false;
    };

    bool get_is_playing() override { return is_playing; };

    void set_loop(bool loop) override
    {
        std::lock_guard<std::mutex> lock(sound_mutex);
        this->looped = loop;
    };
    void set_volume(float vol) override
    {
        std::lock_guard<std::mutex> lock(sound_mutex);
        volume = static_cast<int>(vol * SDL_MIX_MAXVOLUME);

        if (volume > SDL_MIX_MAXVOLUME)
            volume = SDL_MIX_MAXVOLUME;
        else if (volume < 0)
            volume = 0;
    }; // set from 0 to 1

    ~sound_object_impl() override
    {
        std::lock_guard<std::mutex> lock(audio_mutex);

        auto remove_sound =
            std::remove(sound_buffers.begin(), sound_buffers.end(), this);
        if (remove_sound != sound_buffers.end())
        {
            sound_buffers.erase(remove_sound);
        }
    };

    uint8_t*   buffer;
    uint32_t   length;
    uint32_t   current_index;
    bool       is_playing;
    bool       looped;
    int        volume;
    std::mutex sound_mutex;
};

sound_object::~sound_object() = default;

void audio_callback(void* user_data, uint8_t* stream, int stream_size)
{
    std::lock_guard<std::mutex> lock(audio_mutex);

    std::fill_n(stream, stream_size, 0);
    for (auto sound : sound_buffers)
    {
        std::lock_guard<std::mutex> lock_sound(sound->sound_mutex);
        if (sound->is_playing)
        {
            uint32_t current_stream_index = 0;
            while (current_stream_index != stream_size)
            {
                uint8_t* current_stream = &stream[current_stream_index];
                uint32_t rest           = sound->length - sound->current_index;
                uint8_t* current_buff   = &sound->buffer[sound->current_index];

                if (rest <=
                    static_cast<uint32_t>(stream_size) - current_stream_index)
                {
                    SDL_MixAudioFormat(current_stream,
                                       current_buff,
                                       audio_device_spec.format,
                                       rest,
                                       sound->volume);
                    sound->current_index = 0;
                    if (!sound->looped)
                    {
                        sound->is_playing = false;
                        break;
                    }
                    current_stream_index += rest;
                }
                else
                {
                    SDL_MixAudioFormat(current_stream,
                                       current_buff,
                                       audio_device_spec.format,
                                       stream_size - current_stream_index,
                                       sound->volume);
                    sound->current_index += static_cast<uint32_t>(
                        stream_size - current_stream_index);
                    current_stream_index += static_cast<uint32_t>(
                        stream_size - current_stream_index);
                }
            }
        }
    }
}

bool init()
{
    audio_device_spec.freq     = 48000;
    audio_device_spec.format   = SDL_AUDIO_S16LSB;
    audio_device_spec.channels = 2;
    audio_device_spec.samples  = 1024; // must be power of 2
    audio_device_spec.callback = &audio_callback;

    SDL_AudioSpec returned_audio_device_spec;
    const char*   default_audio_device_name = nullptr;
    audio_device_id = SDL_OpenAudioDevice(default_audio_device_name,
                                          0,
                                          &audio_device_spec,
                                          &returned_audio_device_spec,
                                          SDL_AUDIO_ALLOW_ANY_CHANGE);

    audio_device_spec = returned_audio_device_spec;

    if (audio_device_id == 0)
    {
        std::cerr << "Failed to open audio device. Error:" << SDL_GetError()
                  << std::endl;
        return false;
    }

    std::cout << "Audio device selected: " << '\n'
              << "-freq: " << audio_device_spec.freq << '\n'
              << "-format: " << get_sound_format_name(audio_device_spec.format)
              << '\n'
              << "-channels: "
              << static_cast<uint32_t>(audio_device_spec.channels) << '\n'
              << "-samples: " << audio_device_spec.samples << std::endl;

    SDL_PlayAudioDevice(audio_device_id);

    return true;
}

sound_buffer* create_sound_buffer(std::string_view wav_path)
{
    return new sound_buffer_impl(wav_path);
}

sound_object* create_sound_object(sound_buffer* s_buff)
{
    sound_object_impl* s = new sound_object_impl(s_buff);
    {
        std::lock_guard<std::mutex> lock(audio_mutex);
        sound_buffers.push_back(s);
    }
    return s;
}

sound_buffer::~sound_buffer() = default;
} // namespace Kengine::audio
