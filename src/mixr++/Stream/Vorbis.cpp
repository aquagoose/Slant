#include "mixr++/Stream/Vorbis.h"
#include "StringUtils.h"

#include <stdexcept>
#include <stb_vorbis.c>

namespace mixr::Stream {
    Vorbis::Vorbis(const std::string& path) {
        FILE* f;
#ifdef _WIN32
        const auto wpath = ToWString(path);
        if (_wfopen_s(&f, wpath.c_str(), L"rb") != 0)
            throw std::runtime_error("Failed to open file.");
#else
        f = fopen(path.c_str(), "rb");
        if (!f)
            throw std::runtime_error("Failed to open file.");
#endif

        int error;
        _vorbis = stb_vorbis_open_file(f, true, &error, nullptr);
        if (!_vorbis) {
            throw std::runtime_error("Failed to load vorbis: Error code " + std::to_string(error));
        }

        stb_vorbis_info info = stb_vorbis_get_info((stb_vorbis*) _vorbis);

        _format = {
            /* DataType= */ DataType::F32,
            /* SampleRate= */ info.sample_rate,
            /* Channels= */ static_cast<uint8_t>(info.channels)
        };

        _lengthInSamples = stb_vorbis_stream_length_in_samples((stb_vorbis*) _vorbis);
        _currentBufferPos = 0;
    }

    Vorbis::~Vorbis() {
        stb_vorbis_close((stb_vorbis*) _vorbis);
    }

    AudioFormat Vorbis::Format() {
        return _format;
    }

    size_t Vorbis::GetBuffer(uint8_t* buffer, size_t bufferLength) {
        size_t currentIndex = 0;

        while (currentIndex < bufferLength) {
            if (_currentBufferPos == 0)
            {
                float** output;
                int channels;
                size_t frameSize = stb_vorbis_get_frame_float((stb_vorbis*) _vorbis, &channels, &output);

                if (frameSize == 0) {
                    break;
                }

                _buffer.clear();
                _buffer.reserve(frameSize * channels * sizeof(float));
                for (size_t i = 0; i < frameSize; i++) {
                    for (int c = 0; c < channels; c++) {
                        uint32_t sample = *(uint32_t*) &output[c][i];

                        _buffer.push_back(sample & 0xFF);
                        _buffer.push_back(sample >> 8);
                        _buffer.push_back(sample >> 16);
                        _buffer.push_back(sample >> 24);
                    }
                }
            }

            auto copySize = (_buffer.size() - _currentBufferPos);
            if (currentIndex + copySize > bufferLength) {
                copySize = bufferLength - currentIndex;
            }

            std::copy(_buffer.data() + _currentBufferPos, _buffer.data() + _currentBufferPos + copySize, buffer + currentIndex);

            currentIndex += copySize;

            _currentBufferPos += copySize;

            if (_currentBufferPos >= _buffer.size()) {
                _currentBufferPos -= _buffer.size();
            }
        }

        return currentIndex;
    }

    void Vorbis::Restart() {
        stb_vorbis_seek_start((stb_vorbis*) _vorbis);
    }

    void Vorbis::SeekToSample(size_t sample) {
        stb_vorbis_seek((stb_vorbis*) _vorbis, sample);
    }

    size_t Vorbis::PositionInSamples() {
        return static_cast<size_t>(stb_vorbis_get_sample_offset((stb_vorbis*) _vorbis));
    }

    size_t Vorbis::LengthInSamples() {
        return _lengthInSamples;
    }

    std::vector<uint8_t> Vorbis::GetPCM() {
        std::vector<uint8_t> data;
        data.resize(_lengthInSamples * _format.Channels * sizeof(float));

        GetBuffer(data.data(), data.size());

        return data;
    }
}