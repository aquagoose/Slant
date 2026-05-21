#pragma once

#include <fstream>
#include <string>

#include "AudioStream.h"
#include "../Common.h"

namespace Slant::Stream {

    enum ADPCMType {
        IMA
    };

    struct ADPCMInfo {
        ADPCMType Type;
        size_t ChunkSize;
    };

    class SLANT_CPP_API Wav : public AudioStream {
    private:
        std::ifstream _stream;
        AudioFormat _format;
        bool _isAdpcm;
        ADPCMInfo _adpcmInfo;

        size_t _dataStartPoint;
        size_t _currentBufferPos;
        uint32_t _dataLength;

    public:
        explicit Wav(const std::string& path);
        ~Wav() override;

        AudioFormat Format() override;

        size_t GetBuffer(uint8_t *buffer, size_t bufferLength) override;

        void Restart() override;
        void SeekToSample(size_t sample) override;

        size_t PositionInSamples() override;

        size_t LengthInSamples() override;
        std::vector<uint8_t> GetPCM() override;

        bool IsADPCM();
        ADPCMInfo ADPCMInfo();
    };

}
