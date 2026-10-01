#ifndef MIXR_CORE_H
#define MIXR_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

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

const char *mxResultToString(MxResult result);

#ifdef __cplusplus
}
#endif
#endif //MIXR_CORE_H
