#ifndef MIXR_CONTEXT_H
#define MIXR_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"

#include <stdint.h>
#include <stddef.h>

// Performs mixing, creates buffers and sources.
typedef struct MxContext MxContext;
// A buffer contains audio data.
typedef size_t MxBuffer;

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

// Create a mixr context.
MxResult mxCreateContext(const MxContextInfo *info, MxContext **context);
// Destroy a mixr context.
MxResult mxDestroyContext(MxContext *context);

// Create a buffer for the given context.
MxResult mxCreateBuffer(MxContext *context, const MxBufferInfo *info, MxBuffer *buffer);
//MxResult mxDestroyBuffer(MxContext *context, MxBuffer buffer);

#ifdef __cplusplus
}
#endif
#endif //MIXR_CONTEXT_H
