#ifndef MUSIC_H_
#define MUSIC_H_

#include <iostream>
#include <atomic>
#include <cstring>
#include "sndfile.h"
#include "portaudio.h"

class Music
{
public:
    Music(const std::string &filename)
    {
        data.loop = false;
        data.seekBegin = false;

        data.file = sf_open(filename.c_str(), SFM_READ, &data.info);
        if (sf_error(data.file) != SF_ERR_NO_ERROR)
            error("failed to open", filename, sf_strerror(data.file));

        auto device = Pa_GetDefaultOutputDevice();
        if (device == paNoDevice)
            error("no default input device");

        paError = Pa_OpenDefaultStream(&stream, 0, data.info.channels, paFloat32, data.info.samplerate, 512, callback, &data);
        if (paError != paNoError)
            error("failed to open the stream", Pa_GetErrorText(paError));
    }

    void play()
    {
        if (Pa_IsStreamActive(stream))
        {
            paError = Pa_AbortStream(stream);
            if (paError != paNoError)
                error("failed to abort the stream", Pa_GetErrorText(paError));
        }

        data.seekBegin = true;

        paError = Pa_StartStream(stream);
        if (paError != paNoError)
            error("failed to start the stream", Pa_GetErrorText(paError));
    }

    bool isPlaying()
    {
        return Pa_IsStreamActive(stream);
    }

    void abort()
    {
        paError = Pa_AbortStream(stream);
        if (paError != paNoError)
            error("failed to abort the stream", Pa_GetErrorText(paError));
    }

    void setLoop(bool loop = true)
    {
        data.loop = loop;
    }

    ~Music()
    {
        paError = Pa_CloseStream(stream);
        if (paError != paNoError)
            error("failed to close the stream", Pa_GetErrorText(paError));

        sf_close(data.file);
    }

private:
    struct CallbackData
    {
        SNDFILE *file;
        SF_INFO info;
        std::atomic<bool> loop;
        std::atomic<bool> seekBegin;
    };

    SNDFILE *file;
    PaStream *stream;
    PaError paError;
    CallbackData data;

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

        if (readCount < frameCount)
        {
            if (data.loop)
            {
                sf_seek(data.file, 0, SEEK_SET);
                data.seekBegin = false;
                return paContinue;
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
};

#endif /* MUSIC_H_ */
