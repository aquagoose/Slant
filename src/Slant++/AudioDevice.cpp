#include "Slant++/AudioDevice.h"
#include "Impl.h"

#if defined(_WIN32)
#include "Device/SdlDevice.h"
#elif defined(__linux__)
#include "Device/AlsaDevice.h"
#endif

namespace Slant {
    AudioDevice::AudioDevice(uint32_t sampleRate) {
        _context = std::make_unique<Slant::Context>(sampleRate);
    }

    void AudioDevice::GetBuffer(uint8_t* buffer, size_t dataLength) {
        _context->_impl->MixToStereoF32Buffer((float*) buffer, dataLength / 4);
    }

    Context* AudioDevice::Context() {
        return _context.get();
    }

    std::unique_ptr<AudioDevice> AudioDevice::Create(uint32_t sampleRate)
    {
#if defined(_WIN32)
        return std::make_unique<Device::SdlDevice>(sampleRate);
#elif defined(__linux__)
        return std::make_unique<Device::AlsaDevice>(sampleRate);
#endif
    }
}
