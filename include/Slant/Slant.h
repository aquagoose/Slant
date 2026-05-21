#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef _WIN32
#define Slant_C_API __declspec(dllexport)
#else
#define Slant_C_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct MxContext MxContext;
    typedef struct MxDevice MxDevice;

    typedef size_t MxAudioBuffer;
    typedef size_t MxAudioSource;

    typedef enum {
        MX_DATA_TYPE_I8,
        MX_DATA_TYPE_U8,
        MX_DATA_TYPE_I16,
        MX_DATA_TYPE_I32,
        MX_DATA_TYPE_F32
    } MxDataType;

    typedef struct {
        MxDataType DataType;
        uint32_t SampleRate;
        uint8_t Channels;
    } MxAudioFormat;

    typedef enum {
        MX_SOURCE_TYPE_PCM,
        MX_SOURCE_TYPE_ADPCM
    } MxSourceType;

    typedef enum {
        MX_SOURCE_STATE_STOPPED,
        MX_SOURCE_STATE_PAUSED,
        MX_SOURCE_STATE_PLAYING
    } MxSourceState;

    typedef struct {
        size_t ChunkSize;
    } MxADPCMDescription;

    typedef struct {
        MxSourceType Type;
        MxAudioFormat Format;

        union {
            MxADPCMDescription ADPCM;
        };
    } MxSourceDescription;

    Slant_C_API void mxCreateContext(uint32_t sampleRate, MxContext **pContext);
    Slant_C_API void mxDestroyContext(MxContext *context);

    Slant_C_API void mxCreateDevice(uint32_t sampleRate, MxDevice **pDevice);
    Slant_C_API void mxDeviceGetContext(MxDevice *device, MxContext **pContext);
    Slant_C_API void mxDestroyDevice(MxDevice *device);

    Slant_C_API MxAudioBuffer mxContextCreateBuffer(MxContext *context, uint8_t *data, size_t dataLength);
    Slant_C_API void mxContextDestroyBuffer(MxContext *context, MxAudioBuffer buffer);

    Slant_C_API MxAudioSource mxContextCreateSource(MxContext *context, MxSourceDescription *description);
    Slant_C_API void mxContextDestroySource(MxContext *context, MxAudioSource source);

    Slant_C_API void mxContextUpdateBuffer(MxContext *context, MxAudioBuffer buffer, uint8_t *data, size_t dataLength);

    Slant_C_API void mxSourceSubmitBuffer(MxContext *context, MxAudioSource source, MxAudioBuffer buffer);
    Slant_C_API void mxSourceClearBuffers(MxContext *context, MxAudioSource source);
    Slant_C_API void mxSourcePlay(MxContext *context, MxAudioSource source);
    Slant_C_API void mxSourcePause(MxContext *context, MxAudioSource source);
    Slant_C_API void mxSourceStop(MxContext *context, MxAudioSource source);
    Slant_C_API void mxSourceSetSpeed(MxContext *context, MxAudioSource source, double speed);
    Slant_C_API void mxSourceSetVolume(MxContext *context, MxAudioSource source, float volume);
    Slant_C_API void mxSourceSetLooping(MxContext *context, MxAudioSource source, bool looping);
    Slant_C_API void mxSourceSetPanning(MxContext *context, MxAudioSource source, float panning);
    Slant_C_API void mxSourceSetChannelVolumes(MxContext *context, MxAudioSource source, float volumeL, float volumeR);
    Slant_C_API void mxSourceSetBufferFinishedCallback(MxContext *context, MxAudioSource source, void (*callback)(void*), void* userData);
    Slant_C_API void mxSourceSetStateChangedCallback(MxContext *context, MxAudioSource source, void (*callback)(MxSourceState, void*), void* userData);

    Slant_C_API MxSourceState mxSourceGetState(MxContext *context, MxAudioSource source);
    Slant_C_API double mxSourceGetSpeed(MxContext *context, MxAudioSource source);
    Slant_C_API float mxSourceGetVolume(MxContext *context, MxAudioSource source);
    Slant_C_API bool mxSourceGetLooping(MxContext *context, MxAudioSource source);
    Slant_C_API float mxSourceGetPanning(MxContext *context, MxAudioSource source);
    Slant_C_API void mxSourceGetChannelVolumes(MxContext *context, MxAudioSource source, float *volumeL, float *volumeR);
    Slant_C_API size_t mxSourceGetPositionSamples(MxContext *context, MxAudioSource source);
    Slant_C_API double mxSourceGetPositionSeconds(MxContext *context, MxAudioSource source);

    Slant_C_API float mxContextGetMasterVolume(MxContext *context);
    Slant_C_API void mxContextSetMasterVolume(MxContext *context, float volume);

    Slant_C_API void mxContextMixToStereoF32Buffer(MxContext *context, float *buffer, size_t length);

#ifdef __cplusplus
}
#endif