#pragma once

#include <alsa/asoundlib.h>

#include "../../include/Slant++/AudioDevice.h"

namespace Slant::Device
{
    class SLANT_CPP_API AlsaDevice final : public AudioDevice
    {
    private:
        snd_pcm_t* _device;
        size_t _bufferSize;
        size_t _bufferSizeFrames;
        bool _active;
        static void CallbackThread(AlsaDevice* device);

    public:
        AlsaDevice(uint32_t sampleRate);
        ~AlsaDevice() override;
    };
}
