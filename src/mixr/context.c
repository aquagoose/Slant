#include "mixr/context.h"

#include <stdlib.h>

typedef struct MixrContext
{
    uint32_t sampleRate;
} MixrContext;

MxResult mxCreateContext(const MxContextInfo *info, MxContext **context)
{
    if (!info)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    // sample rate cannot be 0
    if (info->sampleRate == 0)
        return MX_RESULT_ERROR_INVALID_PARAMETER;

    MixrContext *ctx = (MixrContext *) malloc(sizeof(MixrContext));
    if (!ctx)
        return MX_RESULT_ERROR_OUT_OF_MEMORY;

    ctx->sampleRate = info->sampleRate;

    *context = (MxContext *) ctx;
    return MX_RESULT_OK;
}

MxResult mxDestroyContext(MxContext *context)
{
    if (!context)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    MixrContext *ctx = (MixrContext *) context;
    free(ctx);

    return MX_RESULT_OK;
}
