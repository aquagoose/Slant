#include "mixr/context.h"

#include <stdlib.h>

typedef struct MixrBuffer
{

} MixrBuffer;

typedef struct MixrContext
{
    uint32_t sampleRate;

    MixrBuffer *buffers;
    size_t buffersLength;
    size_t buffersCapacity;
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
    ctx->buffersCapacity = 16;
    ctx->buffersLength = 0;
    ctx->buffers = (MixrBuffer *) malloc(ctx->buffersCapacity * sizeof(MixrBuffer));

    *context = (MxContext *) ctx;
    return MX_RESULT_OK;
}

MxResult mxDestroyContext(MxContext *context)
{
    if (!context)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    MixrContext *ctx = (MixrContext *) context;
    free(ctx->buffers);
    free(ctx);

    return MX_RESULT_OK;
}

MxResult mxCreateBuffer(MxContext *context, const MxBufferInfo *info, MxBuffer *buffer)
{
    if (!context)
        return MX_RESULT_ERROR_NULL_PARAMETER;
    if (!info)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    MixrContext *ctx = (MixrContext *) context;

    // resize buffers list if needed
    if (ctx->buffersLength + 1 >= ctx->buffersCapacity)
    {
        ctx->buffersCapacity <<= 1;
        MixrBuffer *buffers = (MixrBuffer *) realloc(ctx->buffers, ctx->buffersCapacity * sizeof(MixrBuffer));
        if (!buffers)
            return MX_RESULT_ERROR_OUT_OF_MEMORY;
        ctx->buffers = buffers;
    }

    size_t bufferID = ++ctx->buffersLength;
    *buffer = (MxBuffer) bufferID;

    MixrBuffer buf = {};
    ctx->buffers[bufferID] = buf;

    return MX_RESULT_OK;
}

/*MxResult mxDestroyBuffer(MxContext *context, MxBuffer buffer)
{
    if (!context)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    MixrContext ctx
}*/
