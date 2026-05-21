#pragma once

#include <cstdint>
#include <vector>
#include <deque>
#include <mutex>

#include "Slant++/Context.h"
#include "Slant++/Common.h"

#include "Slant++/Utils/ADPCM.h"

namespace Slant {

    struct Buffer {
        std::vector<uint8_t> Data;
        size_t DataLength;
    };

    struct Source {
        // ------ Source Info -----
        SourceType Type;
        AudioFormat Format;
        int ByteAlign;
        int StereoAlign;
        double SpeedCorrection;

        // ----- Buffering -----
        std::deque<size_t> QueuedBuffers;
        uint8_t* MixBuffer;

        // ----- Playing Info ------
        bool Playing;
        double Speed;
        float MainVolume;
        bool Looping;

        size_t LengthInSamples;

        float VolumeL;
        float VolumeR;

        size_t Position;
        double FinePosition;

        void (*BufferFinishedCallback)(void*);
        void* BufferFinishedUserData;

        void (*StateChangedCallback)(SourceState, void*);
        void* StateChangedUserData;

        // ----- Interpolation -----
        size_t LastPosition;
        float LastSampleL;
        float LastSampleR;

        // ----- IMA ADPCM -----
        size_t LastChunk;
        size_t ChunkSize;
        size_t NumChunks;
    };

    class Impl {
    private:
        uint32_t _sampleRate;
        float _masterVolume;

        std::vector<Buffer> _buffers;
        std::vector<Source> _sources;

        std::deque<size_t> _availableBuffers;
        std::deque<size_t> _availableSources;

        std::mutex _mutex;

        void UpdateSource(Source* source);

    public:
        explicit Impl(uint32_t sampleRate);

        size_t CreateBuffer(uint8_t* data, size_t dataLength);
        void DestroyBuffer(size_t bufferId);

        size_t CreateSource(const SourceDescription& description);
        void DestroySource(size_t sourceId);

        void UpdateBuffer(size_t bufferId, uint8_t* data, size_t dataLength);

        void SourceSubmitBuffer(size_t sourceId, size_t bufferId);
        void SourceClearBuffers(size_t sourceId);
        void SourcePlay(size_t sourceId);
        void SourcePause(size_t sourceId);
        void SourceStop(size_t sourceId, bool useMutex);
        void SourceSetSpeed(size_t sourceId, double speed);
        void SourceSetVolume(size_t sourceId, float volume);
        void SourceSetLooping(size_t sourceId, bool looping);
        void SourceSetPanning(size_t sourceId, float panning);
        void SourceSetChannelVolumes(size_t sourceId, float volumeL, float volumeR);
        void SourceSetBufferFinishedCallback(size_t sourceId, void (*callback)(void*), void* userData);
        void SourceSetStateChangedCallback(size_t sourceId, void (*callback)(SourceState, void*), void* userData);

        SourceState SourceGetState(size_t sourceId);
        double SourceGetSpeed(size_t sourceId);
        float SourceGetVolume(size_t sourceId);
        bool SourceGetLooping(size_t sourceId);
        float SourceGetPanning(size_t sourceId);
        void SourceGetChannelVolumes(size_t sourceId, float* volumeL, float* volumeR);
        size_t SourceGetPositionSamples(size_t sourceId);
        double SourceGetPositionSeconds(size_t sourceId);

        float GetMasterVolume();
        void SetMasterVolume(float volume);

        void MixToStereoF32Buffer(float* buffer, size_t bufferLength);
    };

}
