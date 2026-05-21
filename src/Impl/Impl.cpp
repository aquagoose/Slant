#include <iostream>
#include <string>
#include <algorithm>
#include "Impl.h"

inline float Clamp(float value, float min, float max) {
    return value <= min ? min : value >= max ? max : value;
}

inline float Lerp(float a, float b, float multiplier) {
    return multiplier * (b - a) + a;
}

namespace Slant {
    Impl::Impl(uint32_t sampleRate) {
        _sampleRate = sampleRate;
        _masterVolume = 1.0f;
    }

    size_t Impl::CreateBuffer(uint8_t* data, size_t dataLength) {
        //std::cout << "Create buffer" << std::endl;
        std::lock_guard guard(_mutex);
        Buffer buffer {
           /* .Data = */ std::vector<uint8_t>(data, data + dataLength),
           /* DataLength= */ dataLength
        };

        size_t index;

        if (_availableBuffers.empty()) {
            index = _buffers.size();
            _buffers.push_back(buffer);
        } else {
            index = _availableBuffers.front();
            _availableBuffers.pop_front();
            _buffers[index] = buffer;
        }

        return index;
    }

    void Impl::DestroyBuffer(size_t bufferId) {
        //std::cout << "Destroy buffer" << std::endl;
        std::lock_guard guard(_mutex);
        Buffer* buffer = &_buffers[bufferId];

        // Search to see if the buffer is contained in the source.
        // If it is, remove it. If the source is currently playing the buffer, then stop the source.
        // This is susceptible to race conditions and you are not meant to call this while a source that contains it is
        // playing, but it offers a bit of protection.
        for (size_t i = 0; i < _sources.size(); i++) {
            Source* source = &_sources[i];
            auto queuedBuffers = &source->QueuedBuffers;

            auto loc = std::find(queuedBuffers->begin(), queuedBuffers->end(), bufferId);
            if (loc != queuedBuffers->end()) {
                if (bufferId == queuedBuffers->front()) {
                    SourceStop(i, false);
                }

                queuedBuffers->erase(std::remove(queuedBuffers->begin(), queuedBuffers->end(), bufferId), queuedBuffers->end());
            }
        }

        buffer->Data = {};

        _availableBuffers.push_back(bufferId);
    }

    size_t Impl::CreateSource(const SourceDescription& description) {
        //std::cout << "Create source" << std::endl;
        std::lock_guard guard(_mutex);
        AudioFormat format = description.Format;

        uint8_t byteAlign;
        switch (format.DataType) {
            case DataType::I8:
            case DataType::U8:
                byteAlign = 1;
                break;
            case DataType::I16:
                byteAlign = 2;
                break;
            case DataType::I32:
            case DataType::F32:
                byteAlign = 4;
                break;
        }

        int channels = format.Channels;

        if (channels < 1 || channels > 2) {
            throw std::runtime_error("Unsupported number of channels: " + std::to_string(channels));
        }

        SourceType type = description.Type;
        auto chunkSize = description.ADPCM.ChunkSize;

        uint8_t* buffer = nullptr;

        switch (type)
        {
            case SourceType::PCM:
                break;
            case SourceType::ADPCM:
                buffer = new uint8_t[(chunkSize - (channels * 4)) * 4];
                break;
        }

        Source source {
           /* .Type = */ type,
           /* .Format = */ format,
           /* .ByteAlign = */ byteAlign,
           /* .StereoAlign = */ byteAlign * (channels - 1),
            // Corrects for the sample rate - if the Mixer sample rate is 48khz, and the buffer sample rate is 44.1khz,
            // then this value will be 0.91xyzw, causing the mixer to play the buffer slightly slower to correct the speed.
           /* .SpeedCorrection = */ static_cast<float>(format.SampleRate) / static_cast<float>(_sampleRate),

           /* .QueuedBuffers = */ {},
           /* .MixBuffer = */ buffer,

           /* .Playing = */ false,
           /* .Speed = */ 1.0,
           /* .MainVolume = */ 1.0f,
           /* .Looping = */ false,

           /* .LengthInSamples = */ 0,

           /* .VolumeL = */ 1.0f,
           /* .VolumeR = */ 1.0f,

           /* .Position = */ 0,
           /* .FinePosition = */ 0.0,

           /* .BufferFinishedCallback = */ nullptr,
           /* .BufferFinishedUserData = */ nullptr,

           /* .SourceFinishedCallback = */ nullptr,
           /* .SourceFinishedUserData = */ nullptr,

           /* .LastPosition = */ 0,
           /* .LastSampleL = */ 0.0f,
           /* .LastSampleR = */ 0.0f,
           /* .LastChunk = */ (size_t) -1,

           /* .ChunkSize = */ chunkSize,

        };

        size_t index;

        if (_availableSources.empty()) {
            index = _sources.size();
            _sources.push_back(source);
        } else {
            index = _availableSources.front();
            _availableSources.pop_front();
            _sources[index] = source;
        }

        return index;
    }

    void Impl::DestroySource(size_t sourceId) {
        //std::cout << "Destroy source" << std::endl;
        std::lock_guard guard(_mutex);
        Source* source = &_sources[sourceId];

        // Simple hack, unregister the state changed callback to prevent SourceStop from calling it.
        source->StateChangedCallback = nullptr;
        source->BufferFinishedCallback = nullptr;

        SourceStop(sourceId, false);

        source->QueuedBuffers = {};
        delete source->MixBuffer;

        _availableSources.push_back(sourceId);
    }

    void Impl::UpdateBuffer(size_t bufferId, uint8_t* data, size_t dataLength) {
        //std::cout << "Update buffer" << std::endl;
        std::lock_guard guard(_mutex);
        Buffer* buffer = &_buffers[bufferId];

        // Resize the buffer if it is not big enough, but otherwise, don't.
        // This may use more memory in some situations, but it prevents unnecessary allocations.
        if (dataLength > buffer->Data.size()) {
            buffer->Data.resize(dataLength);
        }

        std::copy(data, data + dataLength, buffer->Data.data());
        buffer->DataLength = dataLength;
    }

    void Impl::SourceSubmitBuffer(size_t sourceId, size_t bufferId) {
        //std::cout << "Submit buffer" << std::endl;
        std::lock_guard guard(_mutex);
        Source* source = &_sources[sourceId];
        source->QueuedBuffers.push_back(bufferId);

        if (source->QueuedBuffers.size() == 1) {
            UpdateSource(source);
        }
    }

    void Impl::SourceClearBuffers(size_t sourceId) {
        //std::cout << "Clear buffers" << std::endl;
        std::lock_guard guard(_mutex);
        SourceStop(sourceId, false);
        _sources[sourceId].QueuedBuffers = {};
    }

    void Impl::SourcePlay(size_t sourceId) {
        //std::cout << "Play" << std::endl;
        Source* source = &_sources[sourceId];
        SourceState state = SourceGetState(sourceId);

        if (source->QueuedBuffers.empty())
            return;

        source->Playing = true;

        if (state != SourceState::Playing && source->StateChangedCallback) {
            source->StateChangedCallback(SourceState::Playing, source->StateChangedUserData);
        }
    }

    void Impl::SourcePause(size_t sourceId) {
        //std::cout << "Pause" << std::endl;
        Source* source = &_sources[sourceId];
        SourceState state = SourceGetState(sourceId);

        source->Playing = false;

        if (state != SourceState::Paused && source->StateChangedCallback) {
            source->StateChangedCallback(SourceState::Paused, source->StateChangedUserData);
        }
    }

    void Impl::SourceStop(size_t sourceId, bool useMutex) {
        //std::cout << "Stop" << std::endl;
        if (useMutex)
            std::lock_guard guard(_mutex);

        Source* source = &_sources[sourceId];
        SourceState state = SourceGetState(sourceId);

        source->Playing = false;
        source->QueuedBuffers.clear();
        source->Position = 0;
        source->LastPosition = 0;
        source->LastSampleL = 0.0f;
        source->LastSampleR = 0.0f;
        source->FinePosition = 0.0;
        source->LastChunk = -1;

        // Only invoke the callback AFTER everything is set for extra safety.
        if (state != SourceState::Stopped && source->StateChangedCallback) {
            source->StateChangedCallback(SourceState::Stopped, source->StateChangedUserData);
        }
    }

    void Impl::SourceSetSpeed(size_t sourceId, double speed) {
        _sources[sourceId].Speed = speed;
    }

    void Impl::SourceSetVolume(size_t sourceId, float volume) {
        _sources[sourceId].MainVolume = volume;
    }

    void Impl::SourceSetLooping(size_t sourceId, bool looping) {
        _sources[sourceId].Looping = looping;
    }

    void Impl::SourceSetPanning(size_t sourceId, float panning) {
        Source* source = &_sources[sourceId];

        source->VolumeL = Clamp(1 - panning, 0.0f, 1.0f);
        source->VolumeR = Clamp(1 - -panning, 0.0f, 1.0f);
    }

    void Impl::SourceSetChannelVolumes(size_t sourceId, float volumeL, float volumeR) {
        Source* source = &_sources[sourceId];

        source->VolumeL = volumeL;
        source->VolumeR = volumeR;
    }

    void Impl::SourceSetBufferFinishedCallback(size_t sourceId, void (*callback)(void*), void* userData) {
        Source* source = &_sources[sourceId];

        source->BufferFinishedCallback = callback;
        source->BufferFinishedUserData = userData;
    }

    void Impl::SourceSetStateChangedCallback(size_t sourceId, void (*callback)(SourceState, void*), void* userData) {
        Source* source = &_sources[sourceId];

        source->StateChangedCallback = callback;
        source->StateChangedUserData = userData;
    }

    SourceState Impl::SourceGetState(size_t sourceId) {
        Source* source = &_sources[sourceId];

        if (source->Playing) {
            return SourceState::Playing;
        } else if (source->Position == 0 && source->QueuedBuffers.empty()) {
            return SourceState::Stopped;
        } else {
            return SourceState::Paused;
        }
    }

    double Impl::SourceGetSpeed(size_t sourceId) {
        return _sources[sourceId].Speed;
    }

    float Impl::SourceGetVolume(size_t sourceId) {
        return _sources[sourceId].MainVolume;
    }

    bool Impl::SourceGetLooping(size_t sourceId) {
        return _sources[sourceId].Looping;
    }

    float Impl::SourceGetPanning(size_t sourceId) {
        Source* source = &_sources[sourceId];

        // -L + R
        // -1 + 1 = Panning 0
        // -0 + 1 = Panning 1
        // etc
        // The returned value may be "incorrect" if you set the channel volumes separately, but panning != channel volumes,
        // they just affect the same values.
        return Clamp(source->VolumeR + -source->VolumeL, -1, 1);
    }

    void Impl::SourceGetChannelVolumes(size_t sourceId, float* volumeL, float* volumeR) {
        Source* source = &_sources[sourceId];

        *volumeL = source->VolumeL;
        *volumeR = source->VolumeR;
    }

    size_t Impl::SourceGetPositionSamples(size_t sourceId) {
        return _sources[sourceId].Position;
    }

    double Impl::SourceGetPositionSeconds(size_t sourceId) {
        Source* source = &_sources[sourceId];

        return (double) source->Position / (double) source->Format.SampleRate;
    }

    float Impl::GetMasterVolume() {
        return _masterVolume;
    }

    void Impl::SetMasterVolume(float volume) {
        _masterVolume = volume;
    }

    inline float GetSample(const uint8_t* data, size_t index, DataType dataType) {
        switch (dataType) {
            case DataType::I8:
                return (float) (int8_t) data[index] / (float) INT8_MAX;
            case DataType::U8:
                return (float) (int8_t) (data[index] - INT8_MAX) / (float) INT8_MAX;
            case DataType::I16:
                return (float) (int16_t) (data[index + 0] | data[index + 1] << 8) / (float) INT16_MAX;
            case DataType::I32:
                return (float) (data[index + 0] | (data[index + 1] << 8) | (data[index + 2] << 16) | (data[index + 3] << 24)) / (float) INT32_MAX;
            case DataType::F32: {
                uint32_t value = (data[index + 0] | (data[index + 1] << 8) | (data[index + 2] << 16) | (data[index + 3] << 24));
                return *(float*) &value;
            }
        }

        return 0;
    }

    void Impl::MixToStereoF32Buffer(float* buffer, size_t bufferLength) {
        //std::cout << "Begin Mixing" << std::endl;
        std::vector<Source*> finishedSources;
        std::vector<size_t> stoppedSources;
        {
            std::lock_guard guard(_mutex);
            for (int i = 0; i < bufferLength; i += 2) {
                buffer[i + 0] = 0;
                buffer[i + 1] = 0;

                // TODO: Optimize this by placing all playing sources into a vector which gets enumerated over.
                for (int s = 0; s < _sources.size(); s++) {
                    //std::cout << "Processing Source " << s << std::endl;
                    Source* source = &_sources[s];

                    //std::cout << "  Source: " << source << ", Playing: " << source->Playing << ", MainVolume: " << source->MainVolume << std::endl;

                    if (!source->Playing || source->MainVolume <= 0.01f)
                        continue;

                    //std::cout << "  Reached point 1" << std::endl;

                    // If the source speed (the speed that the mixer actually plays at) is above 1, then aliasing is
                    // introduced. So, when the source speed is above 1, we instead sample at a multiple of the mixer
                    // sample rate. So if the source speed was 1.1, and the mixer was sampling at 48khz, then it would sample
                    // at 96khz instead.
                    // Then, at the end, we simply remove the samples that aren't needed. Doing it in this way removes aliasing.
                    // TODO: SampleRate * 3 sounds good? Instead of 2 -> 4, does 2 -> 3 -> 4 work?
                    double sourceSpeed = source->SpeedCorrection * source->Speed;
                    int sampleRateMultiplier = std::max((int) sourceSpeed, 1) << 1;
                    sourceSpeed /= sampleRateMultiplier;

                    // TODO: This works but could be optimized more.
                    for (int c = 0; c < sampleRateMultiplier; c++) {
                        Buffer* buf = &_buffers[source->QueuedBuffers.front()];
                        //std::cout << "  Buffer: " << buf << ", DataLength: " << buf->DataLength << std::endl;

                        uint8_t* bufferData = buf->Data.data();
                        AudioFormat* format = &source->Format;

                        //std::cout << "  BufferData: " << bufferData << ", format: " << format << std::endl;

                        size_t bytePosition = source->Position * (source->ByteAlign + source->StereoAlign);

                        float sampleL, sampleR;

                        switch (source->Type) {
                            case SourceType::PCM: {
                                //std::cout << "  Get samples" << std::endl;
                                sampleL = GetSample(bufferData, bytePosition, format->DataType);
                                sampleR = GetSample(bufferData, bytePosition + source->StereoAlign, format->DataType);
                                //std::cout << "  Got samples!" << std::endl;

                                break;
                            }

                                // This makes me uncomfortable..
                                // The fact that it works and *fast* annoys me because now I don't want to change it even though
                                // it could really do with some threading and being less awful.
                            case SourceType::ADPCM: {
                                auto stereo = source->Format.Channels == 2;
                                size_t chunkSize = source->ChunkSize;
                                size_t dataSize = (chunkSize - (stereo ? 8 : 4)) * 4;

                                size_t chunk = bytePosition / dataSize;

                                if (chunk < source->NumChunks && chunk != source->LastChunk) {
                                    source->LastChunk = chunk;
                                    Utils::ADPCM::DecodeIMAChunk(buf->Data.data() + (chunk * chunkSize), chunkSize, source->MixBuffer, stereo);
                                }

                                size_t newBytePosition = bytePosition % dataSize;

                                uint8_t* sBuf = source->MixBuffer;

                                sampleL = (float) (int16_t) (sBuf[newBytePosition + 0] | sBuf[newBytePosition + 1] << 8) / (float) INT16_MAX;
                                sampleR = (float) (int16_t) (sBuf[newBytePosition + 2] | sBuf[newBytePosition + 3] << 8) / (float) INT16_MAX;

                                break;
                            }
                        }

                        if (c == 0) {
                            float lastSampleL = source->LastSampleL;
                            float lastSampleR = source->LastSampleR;

                            float outSampleL = Lerp(lastSampleL, sampleL, (float) source->FinePosition) * source->VolumeL * source->MainVolume;
                            float outSampleR = Lerp(lastSampleR, sampleR, (float) source->FinePosition) * source->VolumeR * source->MainVolume;

                            buffer[i + 0] += Clamp(outSampleL, -1.0f, 1.0f);
                            buffer[i + 1] += Clamp(outSampleR, -1.0f, 1.0f);
                        }

                        source->FinePosition += sourceSpeed;

                        int intPos = (int) source->FinePosition;
                        source->Position += intPos;
                        source->FinePosition -= intPos;

                        if (source->Position != source->LastPosition) {
                            source->LastPosition = source->Position;
                            source->LastSampleL = sampleL;
                            source->LastSampleR = sampleR;
                        }

                        if (source->Position >= source->LengthInSamples) {
                            //std::cout << "  Reached point 2" << std::endl;
                            // Hmm. This doesn't seem ideal.
                            // Right now the library relies on the queue having elements, since it gets the buffer by checking
                            // the front of the queue. Perhaps the "current buffer" should be stored in an index instead.
                            if (source->QueuedBuffers.size() > 1) {
                                //std::cout << "  Reached point 3" << std::endl;
                                finishedSources.push_back(source);

                                //std::cout << "  Pop queue" << std::endl;
                                source->QueuedBuffers.pop_front();
                                source->Position = 0;
                                //std::cout << "  Updating source" << std::endl;
                                UpdateSource(source);
                            } else if (source->Looping) {
                                source->Position -= source->LengthInSamples;
                            } else {
                                //std::cout << "  Stopping source" << std::endl;
                                source->Playing = false;
                                stoppedSources.push_back(s);

                                break;
                            }
                        }
                    }
                }

                buffer[i + 0] = Clamp(buffer[i + 0] * _masterVolume, -1.0f, 1.0f);
                buffer[i + 1] = Clamp(buffer[i + 1] * _masterVolume, -1.0f, 1.0f);
            }
            //std::cout << "Finish Mixing" << std::endl;
        }

        for (const auto source : finishedSources)
        {
            if (source->BufferFinishedCallback) {
                source->BufferFinishedCallback(source->BufferFinishedUserData);
            }
        }

        for (const auto source : stoppedSources)
        {
            SourceStop(source, true);
        }
    }

    void Impl::UpdateSource(Source* source) {
        //std::cout << "Update source" << std::endl;
        auto dataLength = _buffers[source->QueuedBuffers.front()].DataLength;
        auto byteAlign = source->ByteAlign;
        auto channels = source->Format.Channels;

        switch (source->Type) {
            case SourceType::PCM:
                source->LengthInSamples = dataLength / (byteAlign * channels);
                break;

            case SourceType::ADPCM: {
                auto numChunks = dataLength / source->ChunkSize;
                source->NumChunks = numChunks;
                int bytesToRemovePerChunk = channels * 4;
                source->LengthInSamples = ((dataLength * 4) - (bytesToRemovePerChunk * numChunks)) / (byteAlign * channels);
                break;
            }
        }
    }
}