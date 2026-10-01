#include "mixr/context.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct MixrBuffer
{
    // if false, the buffer is not valid and cannot be used
    bool valid;

    void* data;
    size_t dataLength;
    size_t dataCapacity;
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

    size_t bufferID = ctx->buffersLength++;
    *buffer = (MxBuffer) bufferID;

    MixrBuffer buf;
    buf.valid = true;
    buf.data = NULL;
    buf.dataLength = 0;
    buf.dataCapacity = 0;
    ctx->buffers[bufferID] = buf;

    return MX_RESULT_OK;
}

/*MxResult mxDestroyBuffer(MxContext *context, MxBuffer buffer)
{
    if (!context)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    MixrContext ctx
}*/

MxResult mxUpdateBuffer(MxContext *context, MxBuffer buffer, void *data, size_t dataSize)
{
    if (!context)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    MixrContext *ctx = (MixrContext *) context;
    if (buffer >= ctx->buffersLength || !ctx->buffers[buffer].valid)
        return MX_RESULT_ERROR_INVALID_BUFFER;

    MixrBuffer *buf = &ctx->buffers[buffer];

    // if no data, allocate a new buffer
    if (!buf->data)
    {
        buf->data = malloc(dataSize);
        if (!buf->data)
            return MX_RESULT_ERROR_OUT_OF_MEMORY;
        buf->dataCapacity = dataSize;
    }
    // if the data we're copying is larger than the existing buffer, resize it
    // otherwise, the buffer will not be resized.
    else if (dataSize > buf->dataCapacity)
    {
        buf->dataCapacity = dataSize;
        void* newData = realloc(buf->data, dataSize);
        if (!newData)
            return MX_RESULT_ERROR_OUT_OF_MEMORY;
        buf->data = newData;
    }

    buf->dataLength = dataSize;
    memcpy(buf->data, data, dataSize);

    return MX_RESULT_OK;
}
