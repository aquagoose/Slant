#include "mixr/core.h"

const char* mxResultToString(MxResult result)
{
    switch (result)
    {
        case MX_RESULT_OK:
            return "The operation completed successfully.";
        case MX_RESULT_ERROR_UNKNOWN:
            return "An unknown error has occurred.";
        case MX_RESULT_ERROR_NULL_PARAMETER:
            return "A parameter was a null pointer.";
        case MX_RESULT_ERROR_INVALID_PARAMETER:
            return "A parameter was invalid.";
        case MX_RESULT_ERROR_INVALID_BUFFER:
            return "An invalid buffer was provided.";
        case MX_RESULT_ERROR_INVALID_SOURCE:
            return "An invalid source was provided.";
        case MX_RESULT_ERROR_OUT_OF_MEMORY:
            return "The host has run out of memory.";
        case MX_RESULT_ERROR_FILE_NOT_FOUND:
            return "The file was not found.";
        default:
            return "unknown";
    }
}
