#ifndef MIXR_CONTEXT_H
#define MIXR_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"

#include <stdint.h>
#include <stddef.h>

typedef size_t MxSourceFlags;

// Performs mixing, creates buffers and sources.
typedef struct MxContext MxContext;
// A buffer contains audio data.
typedef size_t MxBuffer;
// A source contains a buffer queue and is used to play audio.
typedef size_t MxSource;

// Contains parameters for use in context creation.
typedef struct MxContextInfo
{
    // The sampling rate. Typical values include 44100 (CD quality) and 48000 (DVD quality). This value CANNOT be 0.
    uint32_t sampleRate;
} MxContextInfo;

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

// Create a mixr context.
MxResult mxCreateContext(const MxContextInfo *info, MxContext **context);
// Destroy a mixr context.
MxResult mxDestroyContext(MxContext *context);

// Create a buffer with the given context.
MxResult mxCreateBuffer(MxContext *context, const MxBufferInfo *info, MxBuffer *buffer);
//MxResult mxDestroyBuffer(MxContext *context, MxBuffer buffer);
// Update a buffer, copying the data to the buffer. data can be NULL, and dataSize can be 0.
MxResult mxUpdateBuffer(MxContext *context, MxBuffer buffer, void* data, size_t dataSize);

// Create a source with the given context.
MxResult mxCreateSource(MxContext *context, const MxSourceInfo *info, MxSource *source);

#ifdef __cplusplus
}
#endif
#endif //MIXR_CONTEXT_H
