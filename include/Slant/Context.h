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
typedef struct
{
    size_t id;
} SlBuffer;

// Contains various results and errors that can occur when calling Slant functions.
typedef enum
{
    // Everything worked okay, and there was no issue.
    SL_RESULT_OK,

    // An unknown/undocumented issue occurred. If you see this error, it should be reported.
    SL_RESULT_UNKNOWN_ERROR,

    // An allocation failed, likely due to out of memory.
    SL_RESULT_OUT_OF_MEMORY,

    // An invalid parameter was passed.
    SL_RESULT_INVALID_PARAMETER,

    // An invalid context was passed as a parameter.
    SL_RESULT_INVALID_CONTEXT,

    // An invalid buffer was passed as a parameter.
    SL_RESULT_INVALID_BUFFER,

    // An invalid source was passed as a parameter.
    SL_RESULT_INVALID_SOURCE
} SlResult;

// Information used on context creation.
typedef struct
{
    // The sample rate in Hz. Standard values include 44100 or 48000. This value CANNOT be 0.
    uint32_t sampleRate;
} SlContextInfo;

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

#ifdef __cplusplus
}
#endif

#endif