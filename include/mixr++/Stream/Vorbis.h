#pragma once

#include "AudioStream.h"

#include <string>
#include <iostream>

namespace mixr::Stream {

    class MIXR_CPP_API Vorbis : public AudioStream {
    private:
        void* _vorbis;
        AudioFormat _format;
        size_t _lengthInSamples;
        std::vector<uint8_t> _buffer;
        size_t _currentBufferPos;

    public:
        explicit Vorbis(const std::string& path);
        ~Vorbis() override;

        AudioFormat Format() override;

        size_t GetBuffer(uint8_t* buffer, size_t bufferLength) override;

        void Restart() override;
        void SeekToSample(size_t sample) override;

        size_t PositionInSamples() override;

        size_t LengthInSamples() override;

        std::vector<uint8_t> GetPCM() override;
    };


}