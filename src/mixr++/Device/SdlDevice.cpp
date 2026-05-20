#include "SdlDevice.h"

#include <SDL.h>
#include <stdexcept>
#include <string>

namespace mixr::Device {
    SdlDevice::SdlDevice(uint32_t sampleRate) : AudioDevice(sampleRate) {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
        }

        SDL_AudioSpec spec {};
        spec.freq = static_cast<int>(sampleRate);
        spec.samples = 512;
        spec.channels = 2;
        spec.format = AUDIO_F32;
        spec.userdata = this;
        spec.callback = AudioCallback;

        _device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
        if (!_device) {
            throw std::runtime_error("Failed to open audio device: " + std::string(SDL_GetError()));
        }

        SDL_PauseAudioDevice(_device, 0);
    }

    SdlDevice::~SdlDevice() {
        SDL_CloseAudioDevice(_device);
        SDL_Quit();
    }

    void SdlDevice::AudioCallback(void* userData, uint8_t* buffer, int length) {
        ((SdlDevice*) userData)->GetBuffer(buffer, length);
    }
}