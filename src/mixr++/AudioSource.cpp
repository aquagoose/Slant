#include "mixr++/AudioSource.h"
#include "mixr++/AudioBuffer.h"
#include "Impl.h"

namespace mixr {
    AudioSource::AudioSource(size_t id, Impl* impl) {
        _id = id;
        _impl = impl;
    }

    AudioSource::~AudioSource() {
        _impl->DestroySource(_id);
    }

    void AudioSource::SubmitBuffer(AudioBuffer* buffer) {
        _impl->SourceSubmitBuffer(_id, buffer->ID());
    }

    void AudioSource::ClearBuffers() {
        _impl->SourceClearBuffers(_id);
    }

    double AudioSource::Speed() {
        return _impl->SourceGetSpeed(_id);
    }

    void AudioSource::SetSpeed(double speed) {
        _impl->SourceSetSpeed(_id, speed);
    }


    float AudioSource::Volume() {
        return _impl->SourceGetVolume(_id);
    }

    void AudioSource::SetVolume(float volume) {
        _impl->SourceSetVolume(_id, volume);
    }

    bool AudioSource::Looping() {
        return _impl->SourceGetLooping(_id);
    }

    void AudioSource::SetLooping(bool looping) {
        _impl->SourceSetLooping(_id, looping);
    }

    float AudioSource::Panning() {
        return _impl->SourceGetPanning(_id);
    }

    void AudioSource::SetPanning(float panning) {
        _impl->SourceSetPanning(_id, panning);
    }

    void AudioSource::GetChannelVolumes(float* volumeL, float* volumeR) {
        _impl->SourceGetChannelVolumes(_id, volumeL, volumeR);
    }

    void AudioSource::SetChannelVolumes(float volumeL, float volumeR) {
        _impl->SourceSetChannelVolumes(_id, volumeL, volumeR);
    }

    SourceState AudioSource::State() const {
        return _impl->SourceGetState(_id);
    }

    size_t AudioSource::Position() const {
        return _impl->SourceGetPositionSamples(_id);
    }

    double AudioSource::PositionSecs() const {
        return _impl->SourceGetPositionSeconds(_id);
    }

    void AudioSource::Play() {
        _impl->SourcePlay(_id);
    }

    void AudioSource::Pause() {
        _impl->SourcePause(_id);
    }

    void AudioSource::Stop() {
        _impl->SourceStop(_id, true);
    }

    void AudioSource::SetBufferFinishedCallback(void (*callback)(void*), void* userData) {
        _impl->SourceSetBufferFinishedCallback(_id, callback, userData);
    }

    void AudioSource::SetStateChangedCallback(void (*callback)(SourceState, void*), void* userData) {
        _impl->SourceSetStateChangedCallback(_id, callback, userData);
    }
}