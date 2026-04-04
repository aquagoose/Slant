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
    SL_SOURCE_TYPE_PCM
} SlSourceType;

typedef enum
{
    SL_SOURCE_STATE_STOPPED,
    SL_SOURCE_STATE_PAUSED,
    SL_SOURCE_STATE_PLAYING
} SlSourceState;

typedef enum
{
    SL_SOURCE_PROPERTY_VOLUME,
    SL_SOURCE_PROPERTY_SPEED,
    SL_SOURCE_PROPERTY_STATE,
    SL_SOURCE_PROPERTY_INTERPOLATION_TYPE
} SlSourceProperty;

typedef enum
{
    SL_INTERPOLATION_TYPE_NONE,
    SL_INTERPOLATION_TYPE_LINEAR
} SlInterpolationType;

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
SL_API SlResult slCreateBuffer(SlContext *context, SlBuffer *buffer);
// Update an audio buffer's data.
SL_API SlResult slUpdateBuffer(SlContext *context, SlBuffer buffer, size_t dataSize, const void *data);

SL_API SlResult slCreateSource(SlContext *context, const SlSourceInfo *info, SlSource *source);

SL_API SlResult slSourceGetPropertyf(SlContext *context, SlSource source, SlSourceProperty property, float *value);
SL_API SlResult slSourceGetPropertyi(SlContext *context, SlSource source, SlSourceProperty property, int *value);

SL_API SlResult slSourceSetPropertyf(SlContext *context, SlSource source, SlSourceProperty property, float value);
SL_API SlResult slSourceSetPropertyi(SlContext *context, SlSource source, SlSourceProperty property, int value);

SL_API SlResult slSourceQueueBuffer(SlContext *context, SlSource source, SlBuffer buffer);

SL_API SlResult slSourceGetState(SlContext *context, SlSource source, SlSourceState *state)
{
    return slSourceGetPropertyi(context, source, SL_SOURCE_PROPERTY_STATE, (int *) state);
}

SL_API SlResult slSourcePlay(SlContext *context, SlSource source)
{
    return slSourceSetPropertyi(context, source, SL_SOURCE_PROPERTY_STATE, SL_SOURCE_STATE_PLAYING);
}

SL_API SlResult slSourcePause(SlContext *context, SlSource source)
{
    return slSourceSetPropertyi(context, source, SL_SOURCE_PROPERTY_STATE, SL_SOURCE_STATE_PAUSED);
}

SL_API SlResult slSourceStop(SlContext *context, SlSource source)
{
    return slSourceSetPropertyi(context, source, SL_SOURCE_PROPERTY_STATE, SL_SOURCE_STATE_STOPPED);
}

#ifdef __cplusplus
}
#endif

#endif