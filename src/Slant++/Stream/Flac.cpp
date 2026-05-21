#include "Slant++/Stream/Flac.h"

#include <iostream>
#include <stdexcept>

namespace Slant::Stream {
    class FlacDecoder : public FLAC::Decoder::File {
    public:
        std::vector<uint8_t> Buffer;
        size_t CurrentBufferPos;

        std::vector<uint8_t>* BufferToWriteTo;

        AudioFormat Format;
        size_t MaxBufferSize;
        size_t BytesPerSample;
        size_t TotalSamples;

        ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame* frame, const FLAC__int32* const buffer[]) override {
            auto blockSize = frame->header.blocksize;
            auto channels = frame->header.channels;

            switch (Format.DataType)
            {
                case DataType::I8:
                {
                    for (int i = 0; i < blockSize; i++) {
                        for (int c = 0; c < channels; c++) {
                            uint8_t d = buffer[c][i];
                            BufferToWriteTo->push_back(d);
                        }
                    }

                    break;
                }

                case DataType::I16:
                {
                    for (int i = 0; i < blockSize; i++) {
                        for (int c = 0; c < channels; c++) {
                            uint16_t d = buffer[c][i];
                            BufferToWriteTo->push_back(d & 0xFF);
                            BufferToWriteTo->push_back(d >> 8);
                        }
                    }

                    break;
                }

                case DataType::I32:
                {
                    for (int i = 0; i < blockSize; i++) {
                        for (int c = 0; c < channels; c++) {
                            int32_t d = buffer[c][i];
                            BufferToWriteTo->push_back(d & 0xFF);
                            BufferToWriteTo->push_back(d >> 8);
                            BufferToWriteTo->push_back(d >> 16);
                            BufferToWriteTo->push_back(d >> 24);
                        }
                    }

                    break;
                }
            }

            return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
        }

        void metadata_callback(const ::FLAC__StreamMetadata* metadata) override {
            auto streamInfo = &metadata->data.stream_info;

            DataType type;
            switch (streamInfo->bits_per_sample) {
                case 8:
                    type = DataType::I8;
                    break;

                case 16:
                    type = DataType::I16;
                    break;

                case 32:
                    type = DataType::I32;
                    break;

                default:
                    throw std::runtime_error("Unsupported bits per sample: " + std::to_string(streamInfo->bits_per_sample));
            }

            auto channels = static_cast<uint8_t>(streamInfo->channels);

            Format = {
               /* .DataType = */ type,
               /* .SampleRate = */ streamInfo->sample_rate,
               /* .Channels = */ channels
            };

            BytesPerSample = streamInfo->bits_per_sample / 8;

            MaxBufferSize = streamInfo->min_blocksize * streamInfo->channels * BytesPerSample;
            TotalSamples = streamInfo->total_samples;
            CurrentBufferPos = 0;
        }

        void error_callback(::FLAC__StreamDecoderErrorStatus status) override {
            std::cout << "Error: " << status << std::endl;
        }
    };

    Flac::Flac(const std::string& path) {
        _file = std::make_unique<FlacDecoder>();

        _file->init(path.c_str());
        _file->process_until_end_of_metadata();
    }

    AudioFormat Flac::Format() {
        return dynamic_cast<FlacDecoder*>(_file.get())->Format;
    }

    size_t Flac::GetBuffer(uint8_t* buffer, size_t bufferLength) {
        auto decoder = dynamic_cast<FlacDecoder*>(_file.get());

        auto decoderBuffer = &decoder->Buffer;

        decoderBuffer->reserve(decoder->MaxBufferSize);

        size_t currentIndex = 0;

        while (currentIndex < bufferLength)
        {
            if (decoder->CurrentBufferPos == 0)
            {
                decoderBuffer->clear();
                decoder->BufferToWriteTo = decoderBuffer;
                decoder->process_single();

                if (decoder->get_state() == FLAC__STREAM_DECODER_END_OF_STREAM) {
                    break;
                }
            }

            decoder->BufferToWriteTo = decoderBuffer;

            auto copySize = (decoderBuffer->size() - decoder->CurrentBufferPos);
            if (currentIndex + copySize > bufferLength) {
                copySize = bufferLength - currentIndex;
            }

            std::copy(decoderBuffer->data() + decoder->CurrentBufferPos, decoderBuffer->data() + decoder->CurrentBufferPos + copySize, buffer + currentIndex);

            currentIndex += copySize;

            decoder->CurrentBufferPos += copySize;

            if (decoder->CurrentBufferPos >= decoderBuffer->size()) {
                decoder->CurrentBufferPos -= decoderBuffer->size();
            }
        }

        return currentIndex;
    }

    void Flac::Restart() {
        auto decoder = dynamic_cast<FlacDecoder*>(_file.get());
        decoder->seek_absolute(0);
        decoder->CurrentBufferPos = 0;
    }

    void Flac::SeekToSample(size_t sample) {
        auto decoder = dynamic_cast<FlacDecoder*>(_file.get());
        decoder->seek_absolute(sample);
    }

    size_t Flac::PositionInSamples() {
        auto decoder = dynamic_cast<FlacDecoder*>(_file.get());

        size_t position;
        decoder->get_decode_position((FLAC__uint64*) &position);

        return position;
    }

    size_t Flac::LengthInSamples() {
        return dynamic_cast<FlacDecoder*>(_file.get())->TotalSamples;
    }

    std::vector<uint8_t> Flac::GetPCM() {
        auto decoder = dynamic_cast<FlacDecoder*>(_file.get());

        std::vector<uint8_t> buffer;
        buffer.reserve(decoder->TotalSamples * decoder->Format.Channels * decoder->BytesPerSample);

        decoder->BufferToWriteTo = &buffer;
        decoder->process_until_end_of_stream();
        decoder->BufferToWriteTo = nullptr;

        return buffer;
    }
}