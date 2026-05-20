#pragma once

#include <vector>
#include <cstdint>

#include "../Common.h"

namespace mixr::Stream {

    class MIXR_CPP_API AudioStream {
    public:
        virtual ~AudioStream() = default;

        virtual AudioFormat Format() = 0;

        virtual size_t GetBuffer(uint8_t* buffer, size_t bufferLength) = 0;

        virtual void Restart() = 0;
        virtual void SeekToSample(size_t sample) = 0;

        virtual size_t PositionInSamples() = 0;

        virtual size_t LengthInSamples() = 0;
        virtual std::vector<uint8_t> GetPCM() = 0;
    };

}
