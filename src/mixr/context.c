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

typedef struct MixrSource
{
    // if false, the source is not valid and cannot be used.
    bool valid;
    MxAudioFormat format;
} MixrSource;

typedef struct MixrContext
{
    uint32_t sampleRate;

    MixrBuffer *buffers;
    size_t buffersLength;
    size_t buffersCapacity;

    MixrSource *sources;
    size_t sourcesLength;
    size_t sourcesCapacity;
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

    ctx->sourcesCapacity = 16;
    ctx->sourcesLength = 0;
    ctx->sources = (MixrSource *) malloc(ctx->sourcesCapacity * sizeof(MixrSource));

    *context = (MxContext *) ctx;
    return MX_RESULT_OK;
}

MxResult mxDestroyContext(MxContext *context)
{
    if (!context)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    MixrContext *ctx = (MixrContext *) context;

    for (size_t i = 0; i < ctx->buffersLength; i++)
        free(ctx->buffers[i].data);

    free(ctx->sources);
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
    if (!data && dataSize > 0)
        return MX_RESULT_ERROR_INVALID_PARAMETER;

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

    if (data)
        memcpy(buf->data, data, dataSize);
    else
        buf->data = NULL;

    return MX_RESULT_OK;
}

MxResult mxCreateSource(MxContext *context, const MxSourceInfo *info, MxSource *source)
{
    if (!context)
        return MX_RESULT_ERROR_NULL_PARAMETER;
    if (!info)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    if (info->format.channels == 0 || info->format.channels > 2)
        return MX_RESULT_ERROR_INVALID_PARAMETER;
    if (info->format.sampleRate == 0)
        return MX_RESULT_ERROR_INVALID_PARAMETER;
    if (info->format.type < 0 || info->format.type > MX_DATA_TYPE_F32)
        return MX_RESULT_ERROR_INVALID_PARAMETER;

    MixrContext *ctx = (MixrContext *) context;

    // resize sources if necessary
    if (ctx->sourcesLength + 1 >= ctx->sourcesCapacity)
    {
        ctx->sourcesCapacity <<= 1;
        MixrSource *sources = (MixrSource *) realloc(ctx->sources, ctx->sourcesCapacity * sizeof(MixrSource));
        if (!sources)
            return MX_RESULT_ERROR_OUT_OF_MEMORY;
        ctx->sources = sources;
    }

    size_t sourceID = ctx->sourcesLength++;
    *source = (MxSource) sourceID;
    MixrSource src;
    src.valid = true;
    src.format = info->format;
    ctx->sources[sourceID] = src;

    return MX_RESULT_OK;
}
