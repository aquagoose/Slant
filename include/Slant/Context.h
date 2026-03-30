#ifndef SLANT_CONTEXT_H
#define SLANT_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#ifdef _WIN32
#define SL_API __declspec(dllexport)
#else
#define SL_API
#endif

typedef struct SlContext SlContext;

typedef enum
{
    SL_RESULT_OK,

    SL_RESULT_UNKNOWN_ERROR,
    SL_RESULT_OUT_OF_MEMORY,

    SL_RESULT_INVALID_CONTEXT,
    SL_RESULT_INVALID_BUFFER,
    SL_RESULT_INVALID_SOURCE
} SlResult;

typedef struct
{

} SlContextInfo;

// Create a Slant context.
SL_API SlResult slCreateContext(SlContextInfo *info, SlContext **context);

// Destroy a Slant context.
SL_API void slDestroyContext(SlContext *context);

// Mix to a stereo interleaved float buffer.
SL_API void slContextMixStereoF32(SlContext *context, uint32_t sampleRate, float* buffer, size_t bufferLength);

#ifdef __cplusplus
}
#endif

#endif