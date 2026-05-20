#include "mixr++/Context.h"
#include "mixr++/AudioBuffer.h"
#include "mixr++/AudioSource.h"
#include "Impl.h"

namespace mixr {
    Context::Context(uint32_t sampleRate) {
        _impl = std::make_unique<Impl>(sampleRate);
    }

    Context::~Context() = default;

    std::unique_ptr<AudioBuffer> Context::CreateBuffer(void* data, size_t dataLength) {
        size_t index = _impl->CreateBuffer(static_cast<uint8_t*>(data), dataLength);
        return std::make_unique<AudioBuffer>(index, _impl.get());
    }

    std::unique_ptr<AudioSource> Context::CreateSource(const SourceDescription& description) {
        size_t index = _impl->CreateSource(description);
        return std::make_unique<AudioSource>(index, _impl.get());
    }

    float Context::MasterVolume() const {
        return _impl->GetMasterVolume();
    }

    void Context::SetMasterVolume(float volume) {
        _impl->SetMasterVolume(volume);
    }

    void Context::MixToStereoF32Buffer(float* buffer, size_t length)
    {
        _impl->MixToStereoF32Buffer(buffer, length);
    }
}
