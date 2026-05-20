#pragma once

#include "AudioStream.h"

#include <FLAC++/decoder.h>
#include <memory>
#include <string>

namespace mixr::Stream {

    class MIXR_CPP_API Flac : public AudioStream {
    private:
        std::unique_ptr<FLAC::Decoder::File> _file;

    public:
        explicit Flac(const std::string& path);

        AudioFormat Format() override;

        size_t GetBuffer(uint8_t *buffer, size_t bufferLength) override;

        void Restart() override;
        void SeekToSample(size_t sample) override;

        size_t PositionInSamples() override;

        size_t LengthInSamples() override;
        std::vector<uint8_t> GetPCM() override;
    };

}
