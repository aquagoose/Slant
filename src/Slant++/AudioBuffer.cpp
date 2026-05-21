#include "Slant++/AudioBuffer.h"
#include "Impl.h"

namespace Slant {
    AudioBuffer::AudioBuffer(size_t id, Impl* impl) {
        _id = id;
        _impl = impl;
    }

    AudioBuffer::~AudioBuffer() {
        _impl->DestroyBuffer(_id);
    }

    void AudioBuffer::Update(uint8_t* data, size_t dataLength) {
        _impl->UpdateBuffer(_id, data, dataLength);
    }
}