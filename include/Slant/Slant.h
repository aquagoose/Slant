#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef _WIN32
#define SLANT_C_API __declspec(dllexport)
#else
#define SLANT_C_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct SlContext SlContext;
    typedef struct SlDevice SlDevice;

    typedef size_t SlAudioBuffer;
    typedef size_t SlAudioSource;

    typedef enum {
        SL_DATA_TYPE_I8,
        SL_DATA_TYPE_U8,
        SL_DATA_TYPE_I16,
        SL_DATA_TYPE_I32,
        SL_DATA_TYPE_F32
    } SlDataType;

    typedef struct {
        SlDataType DataType;
        uint32_t SampleRate;
        uint8_t Channels;
    } SlAudioFormat;

    typedef enum {
        SL_SOURCE_TYPE_PCM,
        SL_SOURCE_TYPE_ADPCM
    } SlSourceType;

    typedef enum {
        SL_SOURCE_STATE_STOPPED,
        SL_SOURCE_STATE_PAUSED,
        SL_SOURCE_STATE_PLAYING
    } SlSourceState;

    typedef struct {
        size_t ChunkSize;
    } SlADPCMDescription;

    typedef struct {
        SlSourceType Type;
        SlAudioFormat Format;

        union {
            SlADPCMDescription ADPCM;
        };
    } SlSourceDescription;

    SLANT_C_API void slCreateContext(uint32_t sampleRate, SlContext **pContext);
    SLANT_C_API void slDestroyContext(SlContext *context);

    SLANT_C_API void slCreateDevice(uint32_t sampleRate, SlDevice **pDevice);
    SLANT_C_API void slDeviceGetContext(SlDevice *device, SlContext **pContext);
    SLANT_C_API void slDestroyDevice(SlDevice *device);

    SLANT_C_API SlAudioBuffer slContextCreateBuffer(SlContext *context, uint8_t *data, size_t dataLength);
    SLANT_C_API void slContextDestroyBuffer(SlContext *context, SlAudioBuffer buffer);

    SLANT_C_API SlAudioSource slContextCreateSource(SlContext *context, SlSourceDescription *description);
    SLANT_C_API void slContextDestroySource(SlContext *context, SlAudioSource source);

    SLANT_C_API void slContextUpdateBuffer(SlContext *context, SlAudioBuffer buffer, uint8_t *data, size_t dataLength);

    SLANT_C_API void slSourceSubmitBuffer(SlContext *context, SlAudioSource source, SlAudioBuffer buffer);
    SLANT_C_API void slSourceClearBuffers(SlContext *context, SlAudioSource source);
    SLANT_C_API void slSourcePlay(SlContext *context, SlAudioSource source);
    SLANT_C_API void slSourcePause(SlContext *context, SlAudioSource source);
    SLANT_C_API void slSourceStop(SlContext *context, SlAudioSource source);
    SLANT_C_API void slSourceSetSpeed(SlContext *context, SlAudioSource source, double speed);
    SLANT_C_API void slSourceSetVolume(SlContext *context, SlAudioSource source, float volume);
    SLANT_C_API void slSourceSetLooping(SlContext *context, SlAudioSource source, bool looping);
    SLANT_C_API void slSourceSetPanning(SlContext *context, SlAudioSource source, float panning);
    SLANT_C_API void slSourceSetChannelVolumes(SlContext *context, SlAudioSource source, float volumeL, float volumeR);
    SLANT_C_API void slSourceSetBufferFinishedCallback(SlContext *context, SlAudioSource source, void (*callback)(void*), void* userData);
    SLANT_C_API void slSourceSetStateChangedCallback(SlContext *context, SlAudioSource source, void (*callback)(SlSourceState, void*), void* userData);

    SLANT_C_API SlSourceState slSourceGetState(SlContext *context, SlAudioSource source);
    SLANT_C_API double slSourceGetSpeed(SlContext *context, SlAudioSource source);
    SLANT_C_API float slSourceGetVolume(SlContext *context, SlAudioSource source);
    SLANT_C_API bool slSourceGetLooping(SlContext *context, SlAudioSource source);
    SLANT_C_API float slSourceGetPanning(SlContext *context, SlAudioSource source);
    SLANT_C_API void slSourceGetChannelVolumes(SlContext *context, SlAudioSource source, float *volumeL, float *volumeR);
    SLANT_C_API size_t slSourceGetPositionSamples(SlContext *context, SlAudioSource source);
    SLANT_C_API double slSourceGetPositionSeconds(SlContext *context, SlAudioSource source);

    SLANT_C_API float slContextGetMasterVolume(SlContext *context);
    SLANT_C_API void slContextSetMasterVolume(SlContext *context, float volume);

    SLANT_C_API void slContextMixToStereoF32Buffer(SlContext *context, float *buffer, size_t length);

#ifdef __cplusplus
}
#endif