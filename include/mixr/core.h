#ifndef MIXR_CORE_H
#define MIXR_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum MxResult
{
    // The operation completed successfully.
    MX_RESULT_OK,

    // An unknown error has occurred.
    MX_RESULT_ERROR_UNKNOWN,

    // A parameter was a null pointer.
    MX_RESULT_ERROR_NULL_PARAMETER,

    // A parameter was invalid.
    MX_RESULT_ERROR_INVALID_PARAMETER,

    // An invalid buffer was provided.
    MX_RESULT_ERROR_INVALID_BUFFER,

    // An invalid source was provided.
    MX_RESULT_ERROR_INVALID_SOURCE,

    // The host has run out of memory.
    MX_RESULT_ERROR_OUT_OF_MEMORY,

    // The file was not found.
    MX_RESULT_ERROR_FILE_NOT_FOUND
} MxResult;

// Defines supported data types for audio data.
typedef enum MxDataType
{
    MX_DATA_TYPE_I8,
    MX_DATA_TYPE_U8,
    MX_DATA_TYPE_I16,
    MX_DATA_TYPE_U16,
    MX_DATA_TYPE_I32,
    MX_DATA_TYPE_F32
} MxDataType;

// Describes the format of audio data.
typedef struct MxAudioFormat
{
    MxDataType type;
    uint32_t sampleRate;
    uint8_t channels;
} MxAudioFormat;

const char *mxResultToString(MxResult result);

#ifdef __cplusplus
}
#endif
#endif //MIXR_CORE_H
