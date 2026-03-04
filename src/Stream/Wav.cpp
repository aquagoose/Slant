#include "mixr/Stream/Wav.hpp"
#include "StringUtils.h"

#include <stdexcept>

namespace mixr::Stream {
    Wav::Wav(const std::string& path) {
        _isAdpcm = false;

        constexpr uint32_t riff = 0x46464952;
        constexpr uint32_t wave = 0x45564157;
        constexpr uint32_t fmt  = 0x20746D66;
        constexpr uint32_t data = 0x61746164;

#if _WIN32
        const auto wpath = ToWString(path);
        _stream = std::ifstream(wpath.c_str(), std::ios::in | std::ios::binary);
#else
        _stream = std::ifstream(path, std::ios::in | std::ios::binary);
#endif

        if (!_stream.is_open())
            throw std::runtime_error("Failed to open file!");

        _stream.seekg(0, std::ios::end);
        uint32_t fileSize = _stream.tellg();
        _stream.seekg(0, std::ios::beg);

        uint32_t magic;
        _stream.read((char*) &magic, sizeof(magic));
        if (magic != riff) {
            throw std::runtime_error("Given file is not a wav file: Missing RIFF header.");
        }

        _stream.seekg(4, std::ios::cur); // File size

        uint32_t waveHeader;
        _stream.read((char*) &waveHeader, sizeof(waveHeader));
        if (waveHeader != wave) {
            throw std::runtime_error("Given file is not a valid wav file: Expected WAVE, was not found.");
        }

        while (true) {
            uint32_t header;
            _stream.read((char*) &header, sizeof(header));

            uint32_t chunkSize;
            _stream.read((char*) &chunkSize, sizeof(chunkSize));

            switch (header) {
                case fmt: {
                    uint16_t type;
                    _stream.read((char*) &type, sizeof(type));

                    uint16_t channels;
                    _stream.read((char*) &channels, sizeof(channels));
                    _format.Channels = static_cast<uint8_t>(channels);

                    uint32_t sampleRate;
                    _stream.read((char*) &sampleRate, sizeof(sampleRate));

                    _stream.seekg(4, std::ios::cur); // Average bytes per second. Not needed

                    uint16_t blockAlign;
                    _stream.read((char*) &blockAlign, sizeof(blockAlign));

                    uint16_t bitsPerSample;
                    _stream.read((char*) &bitsPerSample, sizeof(bitsPerSample));

                    _format.SampleRate = sampleRate;

                    switch (type) {
                        case 1: {
                            switch (bitsPerSample) {
                                case 8:
                                    _format.DataType = DataType::U8;
                                    break;

                                case 16:
                                    _format.DataType = DataType::I16;
                                    break;

                                case 32:
                                    _format.DataType = DataType::I32;
                                    break;

                                default:
                                    throw std::runtime_error("Unsupported data type.");
                            }

                            break;
                        }

                        case 3: {
                            switch (bitsPerSample) {
                                case 32:
                                    _format.DataType = DataType::F32;
                                    break;

                                default:
                                    throw std::runtime_error("Unsupported data type.");
                            }

                            break;
                        }

                        /*case 17: {
                            _format.DataType = DataType::I16;
                            _isAdpcm = true;
                            _adpcmInfo = {
                               /* .Type = ADPCMType::IMA,
                               /* .ChunkSize = blockAlign
                            };

                            // MS IMA ADPCM fmt headers contain extra data that we don't need, so just skip over it.
                            // https://wiki.multimedia.cx/index.php/WAVEFORMATEX
                            uint16_t extraDataSize;
                            _stream.read((char*) &extraDataSize, sizeof(extraDataSize));

                            _stream.seekg(extraDataSize, std::ios::cur);

                            break;

                        }*/

                        default:
                            throw std::runtime_error("Unsupported data type.");
                    }

                    break;
                }

                case data: {
                    _dataStartPoint = _stream.tellg();
                    _currentBufferPos = _dataStartPoint;
                    _dataLength = chunkSize;

                    // Solve the problem of some wav files being written incorrectly and having incorrect data sizes
                    // in the headers.
                    // TODO: This is a hack and there may potentially be a better way to solve this.
                    if (_dataLength > fileSize - _dataStartPoint)
                        _dataLength = fileSize - _dataStartPoint;

                    return;
                }

                default:
                    _stream.seekg(chunkSize, std::ios::cur);
                    break;
            }
        }
    }

    AudioFormat Wav::Format() {
        return _format;
    }

    size_t Wav::GetBuffer(uint8_t* buffer, size_t bufferLength) {
        _stream.seekg(_currentBufferPos);

        size_t endPoint = _currentBufferPos + bufferLength;
        size_t dataEnd = _dataStartPoint + _dataLength;
        if (endPoint >= dataEnd) {
            endPoint = dataEnd;
        }

        size_t outputLength = endPoint - _currentBufferPos;

        _stream.read((char*) buffer, outputLength);
        _currentBufferPos += outputLength;

        return outputLength;
    }

    void Wav::Restart() {
        _currentBufferPos = _dataStartPoint;
    }

    void Wav::SeekToSample(size_t sample) {
        _currentBufferPos = _dataStartPoint + (sample * _format.Channels * _format.BytesPerSample());
    }

    size_t Wav::PositionInSamples() {
        return (_currentBufferPos - _dataStartPoint) / _format.Channels / _format.BytesPerSample();
    }

    size_t Wav::LengthInSamples() {
        return _dataLength / _format.Channels / _format.BytesPerSample();
    }

    std::vector<uint8_t> Wav::GetPCM() {
        _stream.seekg(_dataStartPoint);

        std::vector<uint8_t> data(_dataLength);
        _stream.read((char*) data.data(), _dataLength);

        return data;
    }

    bool Wav::IsADPCM() {
        return _isAdpcm;
    }

    ADPCMInfo Wav::ADPCMInfo() {
        return _adpcmInfo;
    }

    Wav::~Wav() = default;
}
