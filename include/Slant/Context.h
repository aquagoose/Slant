#ifndef SLANT_CONTEXT_H
#define SLANT_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"

#include <stdint.h>
#include <stddef.h>

typedef struct SlContext SlContext;

typedef struct
{
    size_t id;
} SlBuffer;

typedef struct
{
    size_t id;
} SlSource;

typedef enum
{
    SL_SOURCE_PCM
} SlSourceType;

typedef enum
{
    SL_STATE_STOPPED,
    SL_STATE_PAUSED,
    SL_STATE_PLAYING
} SlSourceState;

// Information used on context creation.
typedef struct
{
    // The sample rate in Hz. Standard values include 44100 or 48000. This value CANNOT be 0.
    uint32_t sampleRate;
} SlContextInfo;

typedef struct
{
    SlAudioSpec spec;
    SlSourceType type;
} SlSourceInfo;

// Create a Slant context.
SL_API SlResult slCreateContext(const SlContextInfo *info, SlContext **context);
// Destroy a Slant context.
SL_API void slDestroyContext(SlContext *context);

// Mix to a stereo interleaved float buffer.
SL_API void slContextMixStereoF32(SlContext *context, float* buffer, size_t bufferLength);

// Create an audio buffer.
SL_API SlResult slContextCreateBuffer(SlContext *context, SlBuffer *buffer);
// Update an audio buffer's data.
SL_API SlResult slContextUpdateBuffer(SlContext *context, SlBuffer buffer, size_t dataSize, const void *data);

SL_API SlResult slContextCreateSource(SlContext *context, const SlSourceInfo *info, SlSource *source);
SL_API SlResult slContextSourceQueueBuffer(SlContext *context, SlSource source, SlBuffer buffer);
SL_API SlResult slSourceGetState(SlContext *context, SlSource source, SlSourceState *state);

#ifdef __cplusplus
}
#endif

#endif