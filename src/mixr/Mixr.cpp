#include "mixr/mixr.h"
#include "Impl.h"
#include "mixr++/AudioDevice.h"

using namespace mixr;

void mxCreateContext(uint32_t sampleRate, MxContext** pContext) {
    Impl* impl = new Impl(sampleRate);
    *pContext = (MxContext*) impl;
}

void mxDestroyContext(MxContext *context) {
    delete (Impl*) context;
}

MxAudioBuffer mxContextCreateBuffer(MxContext *context, uint8_t *data, size_t dataLength) {
    Impl* impl = (Impl*) context;

    size_t index = impl->CreateBuffer(data, dataLength);
    return index;
}

void mxContextDestroyBuffer(MxContext *context, MxAudioBuffer buffer) {
    Impl* impl = (Impl*) context;

    impl->DestroyBuffer(buffer);
}

MxAudioSource mxContextCreateSource(MxContext *context, MxSourceDescription *description) {
    Impl* impl = (Impl*) context;

    auto desc = reinterpret_cast<SourceDescription*>(description);

    size_t index = impl->CreateSource(*desc);
    return index;
}

void mxContextDestroySource(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    impl->DestroySource(source);
}

void mxContextUpdateBuffer(MxContext *context, MxAudioBuffer buffer, uint8_t *data, size_t dataLength) {
    Impl* impl = (Impl*) context;

    impl->UpdateBuffer(buffer, data, dataLength);
}

void mxSourceSubmitBuffer(MxContext *context, MxAudioSource source, MxAudioBuffer buffer) {
    Impl* impl = (Impl*) context;

    impl->SourceSubmitBuffer(source, buffer);
}

void mxSourceClearBuffers(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    impl->SourceClearBuffers(source);
}

void mxSourcePlay(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    impl->SourcePlay(source);
}

void mxSourcePause(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    impl->SourcePause(source);
}

void mxSourceStop(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    impl->SourceStop(source, true);
}

void mxSourceSetSpeed(MxContext *context, MxAudioSource source, double speed) {
    Impl* impl = (Impl*) context;

    impl->SourceSetSpeed(source, speed);
}

void mxSourceSetVolume(MxContext *context, MxAudioSource source, float volume) {
    Impl* impl = (Impl*) context;

    impl->SourceSetVolume(source, volume);
}

void mxSourceSetLooping(MxContext *context, MxAudioSource source, bool looping) {
    Impl* impl = (Impl*) context;

    impl->SourceSetLooping(source, looping);
}

void mxSourceSetPanning(MxContext *context, MxAudioSource source, float panning) {
    Impl* impl = (Impl*) context;

    impl->SourceSetPanning(source, panning);
}

void mxSourceSetChannelVolumes(MxContext *context, MxAudioSource source, float volumeL, float volumeR) {
    Impl* impl = (Impl*) context;

    impl->SourceSetChannelVolumes(source, volumeL, volumeR);
}

void mxSourceSetBufferFinishedCallback(MxContext *context, MxAudioSource source, void (*callback)(void*), void* userData) {
    Impl* impl = (Impl*) context;

    impl->SourceSetBufferFinishedCallback(source, callback, userData);
}

void mxSourceSetStateChangedCallback(MxContext *context, MxAudioSource source, void (*callback)(MxSourceState, void*), void* userData) {
    Impl* impl = (Impl*) context;

    impl->SourceSetStateChangedCallback(source, reinterpret_cast<void (*)(SourceState, void*)>(callback), userData);
}

MxSourceState mxSourceGetState(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    return static_cast<MxSourceState>(impl->SourceGetState(source));
}

double mxSourceGetSpeed(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetSpeed(source);
}

float mxSourceGetVolume(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetVolume(source);
}

bool mxSourceGetLooping(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetLooping(source);
}

float mxSourceGetPanning(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetPanning(source);
}

void mxSourceGetChannelVolumes(MxContext *context, MxAudioSource source, float *volumeL, float *volumeR) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetChannelVolumes(source, volumeL, volumeR);
}

size_t mxSourceGetPositionSamples(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetPositionSamples(source);
}

double mxSourceGetPositionSeconds(MxContext *context, MxAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetPositionSeconds(source);
}

float mxContextGetMasterVolume(MxContext *context) {
    Impl* impl = (Impl*) context;
    return impl->GetMasterVolume();
}

void mxContextSetMasterVolume(MxContext *context, float volume) {
    Impl* impl = (Impl*) context;

    impl->SetMasterVolume(volume);
}

void mxCreateDevice(uint32_t sampleRate, MxDevice** pDevice)
{
    auto device = AudioDevice::Create(sampleRate);
    *pDevice = (MxDevice*) device.release();
}

void mxDeviceGetContext(MxDevice* device, MxContext** pContext)
{
    auto mxDevice = (AudioDevice*) device;
    *pContext = (MxContext*) mxDevice->Context();
}

void mxDestroyDevice(MxDevice* device)
{
    auto mxDevice = (AudioDevice*) device;
    delete mxDevice;
}

void mxContextMixToStereoF32Buffer(MxContext* context, float* buffer, size_t length)
{
    Impl* impl = (Impl*) context;
    impl->MixToStereoF32Buffer(buffer, length);
}