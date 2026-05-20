#pragma once

#include <cstdint>

#include "Common.h"
#include "AudioBuffer.h"

namespace mixr {

    class MIXR_CPP_API AudioSource {
    private:
        size_t _id;
        Impl* _impl;

    public:
        AudioSource(size_t id, Impl* impl);
        ~AudioSource();

        inline size_t ID() {
            return _id;
        }

        void SubmitBuffer(AudioBuffer* buffer);
        void ClearBuffers();

        double Speed();
        void SetSpeed(double speed);

        float Volume();
        void SetVolume(float volume);

        bool Looping();
        void SetLooping(bool looping);

        float Panning();
        void SetPanning(float panning);

        void GetChannelVolumes(float* volumeL, float* volumeR);
        void SetChannelVolumes(float volumeL, float volumeR);

        void SetBufferFinishedCallback(void (*callback)(void*), void* userData);
        void SetStateChangedCallback(void (*callback)(SourceState, void*), void* userData);

        [[nodiscard]] SourceState State() const;
        size_t Position() const;
        double PositionSecs() const;

        void Play();
        void Pause();
        void Stop();
    };

}
