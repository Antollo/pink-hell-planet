#ifndef MUSIC_H_
#define MUSIC_H_

#include <iostream>
#include <atomic>
#include <cstring>
#include <utility>
#include <cmath>
#include <thread>
#include <functional>
#include <future>
#include <chrono>
#include <glm/glm.hpp>
#include "sndfile.h"
#include "portaudio.h"

class Music
{
public:
    Music() : loaded(false) {}

    Music(const std::string &filename)
        : loaded(false)
    {
        load(filename);
    }

    void load(const std::string &filename)
    {
        if (!loaded)
        {
            loaded = true;
            data.loop = false;
            data.seekBegin = false;
            data.volume = 1.f;
            data.elevate = false;

            data.file = sf_open(filename.c_str(), SFM_READ, &data.info);
            if (sf_error(data.file) != SF_ERR_NO_ERROR)
                error("failed to open", filename, sf_strerror(data.file));

            auto device = Pa_GetDefaultOutputDevice();
            if (device == paNoDevice)
                error("no default output device");

            paError = Pa_OpenDefaultStream(&stream, 0, data.info.channels, paFloat32, data.info.samplerate, 512, callback, &data);
            if (paError != paNoError)
                error("failed to open the stream", Pa_GetErrorText(paError));
        }
        else
            error("music already loadedd");
    }

    void play()
    {
        if (loaded)
        {
            abort();

            data.seekBegin = true;
            if (data.elevate)
            {
                data.volume *= elevationConstant;
                globalVolume = 1.f / elevationConstant;
            }
            paError = Pa_StartStream(stream);
            if (paError != paNoError)
            {
                data.volume /= elevationConstant;
                globalVolume = 1.f;
                data.elevate = false;
                error("failed to start the stream", Pa_GetErrorText(paError));
            }
        }
        else
            error("music not loadedd");
    }

    bool isPlaying()
    {
        return loaded && Pa_IsStreamActive(stream);
    }

    void abort()
    {
        if (loaded && !Pa_IsStreamStopped(stream))
        {
            paError = Pa_AbortStream(stream);
            if (paError != paNoError)
                error("failed to abort the stream", Pa_GetErrorText(paError));
            if (data.elevate)
            {
                data.volume /= elevationConstant;
                globalVolume = 1.f;
                data.elevate = false;
            }
        }
    }

    Music &setLoop(bool loop = true)
    {
        data.loop = loop;
        return *this;
    }

    Music &setVolumeMultiplier(float volume)
    {
        data.volume = volume;
        return *this;
    }

    Music &elevate(bool condition = true)
    {
        data.elevate = true;
        return *this;
    }

    void playDelayed(float seconds)
    {
        std::thread thread([this, seconds]() {
            std::this_thread::sleep_for(std::chrono::duration<float>(seconds));
            play();
        });
        thread.detach();
    }

    ~Music()
    {
        if (loaded)
        {
            paError = Pa_CloseStream(stream);
            if (paError != paNoError)
                error("failed to close the stream", Pa_GetErrorText(paError));

            sf_close(data.file);
        }
    }

    static float volumeMultiplier(float r, const glm::vec3 &v)
    {
        return r * r / glm::dot(v, v);
    }

private:
    struct CallbackData
    {
        SNDFILE *file;
        SF_INFO info;
        float volume;
        bool elevate;
        std::atomic<bool> loop;
        std::atomic<bool> seekBegin;
    };

    PaStream *stream;
    PaError paError;
    CallbackData data;
    bool loaded;

    static int callback(const void *, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *, PaStreamCallbackFlags, void *userData)
    {
        CallbackData &data = *static_cast<CallbackData *>(userData);
        float *out = static_cast<float *>(output);

        std::memset(out, 0, sizeof(float) * frameCount * data.info.channels);

        if (data.seekBegin)
        {
            sf_seek(data.file, 0, SEEK_SET);
            data.seekBegin = false;
        }

        sf_count_t readCount = sf_read_float(data.file, out, frameCount * data.info.channels);

        //const float volumeMultiplier = std::pow(10.f, (data.volume / 10.f));

        if (data.volume * globalVolume != 1.f)
        {
            #pragma omp parallel for num_threads(2)
            for (int i = 0; i < readCount; i++)
                out[i] *= data.volume * globalVolume;
        }

        if (readCount < (long)frameCount)
        {
            if (data.loop)
            {
                sf_seek(data.file, 0, SEEK_SET);
                data.seekBegin = false;
                return paContinue;
            }
            if (data.elevate)
            {
                data.volume /= elevationConstant;
                globalVolume = 1.f;
                data.elevate = false;
            }
            return paComplete;
        }
        return paContinue;
    }

    template <class... Args>
    static void error(Args... args)
    {
        ((std::cerr << args << ' '), ...) << std::endl;
    }

    struct Init
    {
        Init()
        {
            PaError paError = Pa_Initialize();
            if (paError != paNoError)
                error("failed to initialize portaudio", Pa_GetErrorText(paError));
        }

        ~Init()
        {
            PaError paError = Pa_Terminate();
            if (paError != paNoError)
                error("failed to terminate portaudio", Pa_GetErrorText(paError));
        }
    };

    static inline Init init;
    static inline float globalVolume = 1.f;
    static constexpr float elevationConstant = 4.f;
};

class MusicManager
{
public:
    static Music &get(const std::string &name)
    {
        static std::map<std::string, MusicStorage> musicMap;
        if (!musicMap.count(name))
            musicMap.emplace(name, name);
        return musicMap[name];
    }

private:
    class MusicStorage
    {
    public:
        MusicStorage() : i(0) {}
        MusicStorage(const std::string &name)
            : i(0)
        {
            for (auto &music : data)
                music.load(name);
        }
        operator Music &()
        {
            return data[i = (i == size - 1) ? 0 : i + 1];
        }

    private:
        static constexpr size_t size = 4;
        std::array<Music, size> data;
        size_t i;
    };
};
#endif /* MUSIC_H_ */
