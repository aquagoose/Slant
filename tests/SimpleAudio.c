#include <Slant/Slant.h>
#include <SDL3/SDL.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "Utils/Utils.h"

void AudioStream(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount)
{
    float buffer[512];
    while (total_amount > 0)
    {
        SlContext *context = (SlContext *) userdata;
        slContextMixStereoF32(context, buffer, 512);

        SDL_PutAudioStreamData(stream, buffer, 512 * sizeof(float));
        total_amount -= 512 * sizeof(float);
    }
}

int main(int argc, char **argv)
{
    ENSURE_ARGS(1, argc, "Please provide the path to a raw PCM file.");

    if (!SDL_Init(SDL_INIT_AUDIO))
    {
        printf("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SlContextInfo contextInfo;
    contextInfo.sampleRate = 48000;

    SlContext* context;
    SlResult result = slCreateContext(&contextInfo, &context);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to create context! %s\n", slResultToString(result));
        return 1;
    }

    SlBuffer buffer;
    result = slContextCreateBuffer(context, &buffer);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to create buffer! %s\n", slResultToString(result));
        return 1;
    }

    size_t dataLength;
    uint8_t* data = LoadFileToBuffer(argv[1], &dataLength);
    if (!data)
    {
        printf("Failed to load file!\n");
        return 1;
    }

    result = slContextUpdateBuffer(context, buffer, dataLength, data);
    free(data);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to update buffer! %s\n", slResultToString(result));
        return 1;
    }

    SlSourceInfo sourceInfo;
    sourceInfo.spec.dataFormat = SL_FORMAT_I16;
    sourceInfo.spec.sampleRate = 44100;
    sourceInfo.spec.channels = 2;
    sourceInfo.type = SL_SOURCE_PCM;

    SlSource source;
    result = slContextCreateSource(context, &sourceInfo, &source);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to create source! %s\n", slResultToString(result));
        return 1;
    }

    slContextSourceQueueBuffer(context, source, buffer);

    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 2;
    spec.freq = (int) contextInfo.sampleRate;

    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, AudioStream, context);
    SDL_ResumeAudioStreamDevice(stream);

    sleep(120);

    SDL_DestroyAudioStream(stream);
    SDL_Quit();
    slDestroyContext(context);
    return 0;
}