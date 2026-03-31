#include "Slant/Context.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define CHECK_CONTEXT(ctx) if (!ctx) return SL_RESULT_INVALID_CONTEXT;
#define CHECK_BUFFER(ctx, buffer) if (buffer.id > ctx->buffersLength || !ctx->buffers[buffer.id].valid) return SL_RESULT_INVALID_BUFFER;

#define INITIAL_CAPACITY 16

typedef struct
{
    // As buffers are never deleted from the buffers list once created (and are instead reused), we must ensure a buffer
    // is valid before it can be used.
    bool valid;
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
    bool valid;
    SlAudioSpec spec;
} SlantSource;

typedef struct
{
    uint32_t sampleRate;
    float masterVolume;

    SlantBuffer *buffers;
    size_t buffersLength;
    size_t buffersCapacity;

    SlantSource *sources;
    size_t sourcesLength;
    size_t sourcesCapacity;
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

    ctx->sources = (SlantSource *) malloc(INITIAL_CAPACITY * sizeof(SlantSource));
    ctx->sourcesLength = 0;
    ctx->sourcesCapacity = INITIAL_CAPACITY;
    if (!ctx->sources)
    {
        *context = NULL;
        free(ctx->buffers);
        free(ctx);
        return SL_RESULT_OUT_OF_MEMORY;
    }

    *context = (SlContext *) ctx;
    return SL_RESULT_OK;
}

void slDestroyContext(SlContext *context)
{
    SlantContext *ctx = (SlantContext *) context;

    for (size_t i = 0; i < ctx->sourcesLength; i++)
    {
        // TODO: For later
    }
    free(ctx->sources);

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
    // Buffer must be valid! Other functions will fail if the buffer is not valid.
    buf.valid = true;
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

    // The buffer ID is simply the index in the array of the buffer.
    // TODO: Buffer reuse
    const size_t id = ctx->buffersLength;
    ctx->buffersLength++;
    ctx->buffers[id] = buf;

    buffer->id = id;
    return SL_RESULT_OK;
}

SlResult slContextCreateSource(SlContext *context, const SlSourceInfo *info, SlSource *source)
{
    CHECK_CONTEXT(context);
    SlantContext *ctx = (SlantContext *) context;

    if (info->type != SL_SOURCE_PCM)
        return SL_RESULT_INVALID_PARAMETER;

    SlantSource src;
    src.valid = true;
    src.spec = info->spec;

    // Resize sources array
    if (ctx->sourcesLength + 1 >= ctx->sourcesCapacity)
    {
        const size_t newCapacity = ctx->sourcesCapacity << 1;
        SlantSource *newSourceArray = (SlantSource *) realloc(ctx->sources, newCapacity * sizeof(SlantSource));
        if (!newSourceArray)
            return SL_RESULT_OUT_OF_MEMORY;
        ctx->sources = newSourceArray;
        ctx->sourcesCapacity = newCapacity;
    }

    const size_t id = ctx->sourcesLength;
    ctx->sourcesLength++;
    ctx->sources[id] = src;

    source->id = id;
    return SL_RESULT_OK;
}

SlResult slContextUpdateBuffer(SlContext* context, const SlBuffer buffer, const size_t dataSize, const void* data)
{
    CHECK_CONTEXT(context);
    SlantContext *ctx = (SlantContext *) context;

    CHECK_BUFFER(ctx, buffer);

    SlantBuffer* buf = &ctx->buffers[buffer.id];
    // Create the buffer's data array if it doesn't exist (if the buffer has not been used)
    if (!buf->data)
    {
        buf->data = (uint8_t *) malloc(dataSize);
        if (!buf->data)
            return SL_RESULT_OUT_OF_MEMORY;
        buf->dataCapacity = dataSize;
    }
    // Only resize the data buffer if it is smaller than the incoming data.
    else if (buf->dataCapacity < dataSize)
    {
        uint8_t *reallocBufferData = realloc(buf->data, dataSize);
        if (!reallocBufferData)
            return SL_RESULT_OUT_OF_MEMORY;
        buf->data = reallocBufferData;
        buf->dataCapacity = dataSize;
    }

    buf->dataLength = dataSize;
    memcpy(buf->data, data, dataSize);

    return SL_RESULT_OK;
}
