#ifndef SLANT_COMMON_H
#define SLANT_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef _WIN32
#define SL_API __declspec(dllexport)
#else
#define SL_API
#endif

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

typedef enum
{
    SL_FORMAT_I8 = 0,
    SL_FORMAT_U8 = 1,
    SL_FORMAT_I16 = 2,
    //SL_FORMAT_I24 = 3,
    SL_FORMAT_I32 = 4,
    SL_FORMAT_F32 = 5,
} SlDataFormat;

// TODO: AudioSpec is a good name? Hmmmm
typedef struct
{
    SlDataFormat dataFormat;
    uint32_t sampleRate;
    uint8_t channels;
} SlAudioSpec;

SL_API const char* slResultToString(SlResult result);

#ifdef __cplusplus
}
#endif

#endif
