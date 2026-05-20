#pragma once

#include "AudioStream.h"

#include <string>

#include <minimp3_ex.h>

namespace mixr::Stream {

    class MIXR_CPP_API Mp3 : public AudioStream {
    private:
        mp3dec_ex_t _mp3;

        AudioFormat _format;
        size_t _lengthInSamples;

    public:
        explicit Mp3(const std::string& path);
        ~Mp3() override;

        AudioFormat Format() override;

        size_t GetBuffer(uint8_t* buffer, size_t bufferLength) override;

        void Restart() override;
        void SeekToSample(size_t sample) override;

        size_t PositionInSamples() override;

        size_t LengthInSamples() override;

        std::vector<uint8_t> GetPCM() override;
    };

}