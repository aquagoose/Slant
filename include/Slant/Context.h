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

// Contains various results and errors that can occur when calling Slant functions.
typedef enum
{
    // Everything worked okay, and there was no issue.
    SL_RESULT_OK,

    // An unknown/undocumented issue occurred. If you see this error, it should be reported.
    SL_RESULT_UNKNOWN_ERROR,

    // An allocation failed, likely due to out of memory.
    SL_RESULT_OUT_OF_MEMORY,

    // An invalid context was passed as a parameter.
    SL_RESULT_INVALID_CONTEXT,

    // An invalid buffer was passed as a parameter.
    SL_RESULT_INVALID_BUFFER,

    // An invalid source was passed as a parameter.
    SL_RESULT_INVALID_SOURCE
} SlResult;

// Information used on context creation.
// Currently, this is empty, but this is reserved for future use.
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