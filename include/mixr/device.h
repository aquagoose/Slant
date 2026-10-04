#ifndef MIXR_DEVICE_H
#define MIXR_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"

#include <stdint.h>
#include <stddef.h>

typedef size_t MxDeviceFlags;

typedef size_t MxSourceFlags;

// Performs mixing, creates buffers and sources.
typedef struct MxDevice MxDevice;
// A buffer contains audio data.
typedef size_t MxBuffer;
// A source contains a buffer queue and is used to play audio.
typedef size_t MxSource;

typedef enum MxDeviceBackend
{
    // Choose a device automatically.
    MX_DEVICE_AUTO,
    // Do not open an audio device. This allows for devices to be used with custom backends, or to be used as software mixers.
    MX_DEVICE_NONE,
    // SDL3 backend.
    MX_DEVICE_SDL
} MxDeviceBackend;

// Contains parameters for use in device creation.
typedef struct MxDeviceInfo
{
    // Which backend to use.
    MxDeviceBackend backend;
    // Device creation flags.
    MxDeviceFlags flags;
    // The sampling rate. Typical values include 44100 (CD quality) and 48000 (DVD quality). This value CANNOT be 0.
    uint32_t sampleRate;
} MxDeviceInfo;

// Contains parameters for use in buffer creation.
typedef struct MxBufferInfo
{

} MxBufferInfo;

// Contains parameters for use in source creation.
typedef struct MxSourceInfo
{
    MxSourceFlags flags;
    MxAudioFormat format;
} MxSourceInfo;

// Create a device.
MxResult mxCreateDevice(const MxDeviceInfo *info, MxDevice **device);
// Destroy a device.
MxResult mxDestroyDevice(MxDevice *device);

// Create a buffer with the given device.
MxResult mxCreateBuffer(MxDevice *device, const MxBufferInfo *info, MxBuffer *buffer);
//MxResult mxDestroyBuffer(MxDevice *device, MxBuffer buffer);
// Update a buffer, copying the data to the buffer. data can be NULL, and dataSize can be 0.
MxResult mxUpdateBuffer(MxDevice *device, MxBuffer buffer, void* data, size_t dataSize);

// Create a source with the given device.
MxResult mxCreateSource(MxDevice *device, const MxSourceInfo *info, MxSource *source);

#ifdef __cplusplus
}
#endif
#endif //MIXR_DEVICE_H
