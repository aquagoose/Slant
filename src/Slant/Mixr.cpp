#include "Slant/Slant.h"
#include "Impl.h"
#include "Slant++/AudioDevice.h"

using namespace Slant;

void slCreateContext(uint32_t sampleRate, SlContext** pContext) {
    Impl* impl = new Impl(sampleRate);
    *pContext = (SlContext*) impl;
}

void slDestroyContext(SlContext *context) {
    delete (Impl*) context;
}

SlAudioBuffer slContextCreateBuffer(SlContext *context, uint8_t *data, size_t dataLength) {
    Impl* impl = (Impl*) context;

    size_t index = impl->CreateBuffer(data, dataLength);
    return index;
}

void slContextDestroyBuffer(SlContext *context, SlAudioBuffer buffer) {
    Impl* impl = (Impl*) context;

    impl->DestroyBuffer(buffer);
}

SlAudioSource slContextCreateSource(SlContext *context, SlSourceDescription *description) {
    Impl* impl = (Impl*) context;

    auto desc = reinterpret_cast<SourceDescription*>(description);

    size_t index = impl->CreateSource(*desc);
    return index;
}

void slContextDestroySource(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    impl->DestroySource(source);
}

void slContextUpdateBuffer(SlContext *context, SlAudioBuffer buffer, uint8_t *data, size_t dataLength) {
    Impl* impl = (Impl*) context;

    impl->UpdateBuffer(buffer, data, dataLength);
}

void slSourceSubmitBuffer(SlContext *context, SlAudioSource source, SlAudioBuffer buffer) {
    Impl* impl = (Impl*) context;

    impl->SourceSubmitBuffer(source, buffer);
}

void slSourceClearBuffers(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    impl->SourceClearBuffers(source);
}

void slSourcePlay(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    impl->SourcePlay(source);
}

void slSourcePause(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    impl->SourcePause(source);
}

void slSourceStop(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    impl->SourceStop(source, true);
}

void slSourceSetSpeed(SlContext *context, SlAudioSource source, double speed) {
    Impl* impl = (Impl*) context;

    impl->SourceSetSpeed(source, speed);
}

void slSourceSetVolume(SlContext *context, SlAudioSource source, float volume) {
    Impl* impl = (Impl*) context;

    impl->SourceSetVolume(source, volume);
}

void slSourceSetLooping(SlContext *context, SlAudioSource source, bool looping) {
    Impl* impl = (Impl*) context;

    impl->SourceSetLooping(source, looping);
}

void slSourceSetPanning(SlContext *context, SlAudioSource source, float panning) {
    Impl* impl = (Impl*) context;

    impl->SourceSetPanning(source, panning);
}

void slSourceSetChannelVolumes(SlContext *context, SlAudioSource source, float volumeL, float volumeR) {
    Impl* impl = (Impl*) context;

    impl->SourceSetChannelVolumes(source, volumeL, volumeR);
}

void slSourceSetBufferFinishedCallback(SlContext *context, SlAudioSource source, void (*callback)(void*), void* userData) {
    Impl* impl = (Impl*) context;

    impl->SourceSetBufferFinishedCallback(source, callback, userData);
}

void slSourceSetStateChangedCallback(SlContext *context, SlAudioSource source, void (*callback)(SlSourceState, void*), void* userData) {
    Impl* impl = (Impl*) context;

    impl->SourceSetStateChangedCallback(source, reinterpret_cast<void (*)(SourceState, void*)>(callback), userData);
}

SlSourceState slSourceGetState(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    return static_cast<SlSourceState>(impl->SourceGetState(source));
}

double slSourceGetSpeed(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetSpeed(source);
}

float slSourceGetVolume(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetVolume(source);
}

bool slSourceGetLooping(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetLooping(source);
}

float slSourceGetPanning(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetPanning(source);
}

void slSourceGetChannelVolumes(SlContext *context, SlAudioSource source, float *volumeL, float *volumeR) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetChannelVolumes(source, volumeL, volumeR);
}

size_t slSourceGetPositionSamples(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetPositionSamples(source);
}

double slSourceGetPositionSeconds(SlContext *context, SlAudioSource source) {
    Impl* impl = (Impl*) context;

    return impl->SourceGetPositionSeconds(source);
}

float slContextGetMasterVolume(SlContext *context) {
    Impl* impl = (Impl*) context;
    return impl->GetMasterVolume();
}

void slContextSetMasterVolume(SlContext *context, float volume) {
    Impl* impl = (Impl*) context;

    impl->SetMasterVolume(volume);
}

void slCreateDevice(uint32_t sampleRate, SlDevice** pDevice)
{
    auto device = AudioDevice::Create(sampleRate);
    *pDevice = (SlDevice*) device.release();
}

void slDeviceGetContext(SlDevice* device, SlContext** pContext)
{
    auto slDevice = (AudioDevice*) device;
    *pContext = (SlContext*) slDevice->Context()->GetImpl();
}

void slDestroyDevice(SlDevice* device)
{
    auto slDevice = (AudioDevice*) device;
    delete slDevice;
}

void slContextMixToStereoF32Buffer(SlContext* context, float* buffer, size_t length)
{
    Impl* impl = (Impl*) context;
    impl->MixToStereoF32Buffer(buffer, length);
}