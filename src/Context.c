#include "Slant/Context.h"

#include <stdlib.h>

#define CHECK_CONTEXT(ctx) if (!ctx) return SL_RESULT_INVALID_CONTEXT;

#define INITIAL_CAPACITY 16

typedef struct
{
    uint8_t* data;
    size_t dataLength;
    // The data capacity and length may not be the same
    // For slightly extra efficiency when updating buffers, do not realloc the data if we use a buffer that is the same
    // size, or smaller than the current buffer size.
    // TODO: Perhaps just have the user pass in a "bufferSize" param in slContextCreateBuffer?
    size_t dataCapacity;
} SlantBuffer;

typedef struct
{
    uint32_t sampleRate;
    float masterVolume;

    SlantBuffer *buffers;
    size_t buffersLength;
    size_t buffersCapacity;
} SlantContext;

SlResult slCreateContext(const SlContextInfo *info, SlContext **context)
{
    // It's likely that if a SR of 0 is passed then the user forgot to set the parameter.
    if (info->sampleRate == 0)
        return SL_RESULT_INVALID_PARAMETER;

    SlantContext *ctx = (SlantContext *) malloc(sizeof(SlantContext));
    if (!ctx)
    {
        *context = NULL;
        return SL_RESULT_OUT_OF_MEMORY;
    }

    ctx->sampleRate = info->sampleRate;
    ctx->masterVolume = 1.0f;

    ctx->buffers = (SlantBuffer *) malloc(INITIAL_CAPACITY * sizeof(SlantBuffer));
    ctx->buffersLength = 0;
    ctx->buffersCapacity = INITIAL_CAPACITY;
    if (!ctx->buffers)
    {
        *context = NULL;
        free(ctx);
        return SL_RESULT_OUT_OF_MEMORY;
    }

    *context = (SlContext *) ctx;
    return SL_RESULT_OK;
}

void slDestroyContext(SlContext *context)
{
    SlantContext *ctx = (SlantContext *) context;

    for (size_t i = 0; i < ctx->buffersLength; i++)
    {
        if (ctx->buffers[i].data)
            free(ctx->buffers[i].data);
    }
    free(ctx->buffers);

    free(ctx);
}

void slContextMixStereoF32(SlContext *context, float *buffer, size_t bufferLength)
{
    SlantContext *ctx = (SlantContext *) context;
}

SlResult slContextCreateBuffer(SlContext *context, SlBuffer *buffer)
{
    CHECK_CONTEXT(context);

    SlantContext *ctx = (SlantContext *) context;

    SlantBuffer buf;
    buf.data = NULL;
    buf.dataLength = 0;
    buf.dataCapacity = 0;

    // Resize array if needed.
    if (ctx->buffersLength + 1 >= ctx->buffersCapacity)
    {
        const size_t newCapacity = ctx->buffersCapacity << 1;
        SlantBuffer *reallocBuffers = (SlantBuffer *) realloc(ctx->buffers, newCapacity * sizeof(SlantBuffer));
        if (!reallocBuffers)
            return SL_RESULT_OUT_OF_MEMORY;
        ctx->buffers = reallocBuffers;
        ctx->buffersCapacity = newCapacity;
    }

    const size_t id = ctx->buffersLength;
    ctx->buffersLength++;
    ctx->buffers[id] = buf;

    buffer->id = id;
    return SL_RESULT_OK;
}
