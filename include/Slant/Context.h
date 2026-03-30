#ifndef SLANT_CONTEXT_H
#define SLANT_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef _WIN32
#define SL_API __declspec(dllexport)
#else
#define SL_API
#endif

typedef struct SlContext SlContext;

typedef enum
{
    SL_RESULT_OK,
    SL_RESULT_UNKNOWN_ERROR
} SlResult;

typedef struct
{
    uint32_t sampleRate;
} SlContextInfo;

// Create a Slant context.
SL_API SlResult slCreateContext(SlContextInfo *info, SlContext **context);
SL_API void slDestroyContext(SlContext *context);

#ifdef __cplusplus
}
#endif

#endif