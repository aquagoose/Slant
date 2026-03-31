#include <Slant/Slant.h>
#include <SDL3/SDL.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void AudioStream(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount)
{
    float buffer[512];
    while (total_amount > 0)
    {
        for (int i = 0; i < 512; i++)
            buffer[i] = (float) rand() / (float) RAND_MAX;

        SDL_PutAudioStreamData(stream, buffer, 512);
        total_amount -= 512;
    }
}

int main(int argc, char **argv)
{
    if (!SDL_Init(SDL_INIT_AUDIO))
    {
        printf("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SlContextInfo contextInfo;
    contextInfo.sampleRate = 44100;

    SlContext* context;
    slCreateContext(&contextInfo, &context);

    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 2;
    spec.freq = (int) contextInfo.sampleRate;

    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, AudioStream, context);
    SDL_ResumeAudioStreamDevice(stream);

    sleep(10);

    SDL_DestroyAudioStream(stream);
    SDL_Quit();
    slDestroyContext(context);
    return 0;
}