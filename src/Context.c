#include "Slant/Context.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INITIAL_CAPACITY 16

#define CHECK_CONTEXT(ctx) if (!ctx) return SL_RESULT_INVALID_CONTEXT;
#define CHECK_BUFFER(ctx, buffer) if (buffer.id > ctx->buffersLength || !ctx->buffers[buffer.id].valid) return SL_RESULT_INVALID_BUFFER;
#define CHECK_SOURCE(ctx, source) if (source.id > ctx->sourcesLength || !ctx->sources[source.id].valid) return SL_RESULT_INVALID_SOURCE;

#define LERP(a, b, amount) (amount * (b - a)) + a;

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
    // Ensures the base speed of 1.0 is correct regardless of sample rate.
    // For example, if spec.sampleRate = 44100, and context.sampleRate = 48000, then this value will be ~0.9
    double speedAdjust;
    // Size in bytes of a single channel.
    size_t channelStride;
    // Size of one sample in bytes. A sample in Slant includes ALL channels.
    size_t sampleStride;

    // The currently queued buffers to play.
    size_t *queuedBuffers;
    size_t queuedBuffersCapacity;
    size_t queuedBuffersFront;
    size_t queuedBuffersBack;

    // The length of the current buffer, in samples.
    size_t currentBufferLengthInSamples;

    // The current playing state
    bool playing;

    // The current sample position
    size_t position;
    double finePosition;
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
        free(ctx->sources[i].queuedBuffers);
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

float GetSample(const SlDataFormat format, const size_t bytePos, const uint8_t *buffer)
{
    switch (format)
    {
        case SL_FORMAT_I8:
            return (float) (int8_t) buffer[bytePos] / (float) INT8_MAX;
        case SL_FORMAT_U8:
            return (float) ((int16_t) (buffer[bytePos] - 128)) / (float) INT8_MAX;
        case SL_FORMAT_I16:
            return (float) (int16_t) (buffer[bytePos] | buffer[bytePos + 1] << 8) / (float) INT16_MAX;
        case SL_FORMAT_I32:
            return (float) (int32_t) ((buffer[bytePos]) | (buffer[bytePos + 1] << 8) | (buffer[bytePos + 2] << 16) | (buffer[bytePos + 3] << 24)) / (float) INT32_MAX;
        case SL_FORMAT_F32:
        {
            uint32_t value = (uint32_t) ((buffer[bytePos]) | (buffer[bytePos + 1] << 8) | (buffer[bytePos + 2] << 16) | (buffer[bytePos + 3] << 24));
            return *(float*) &value;
        }
    }

    return 0;
}

void slContextMixStereoF32(SlContext *context, float *buffer, size_t bufferLength)
{
    SlantContext *ctx = (SlantContext *) context;

    for (size_t i = 0; i < bufferLength; i += 2)
    {
        buffer[i + 0] = 0.0f;
        buffer[i + 1] = 0.0f;

        for (size_t src = 0; src < ctx->sourcesLength; src++)
        {
            SlantSource *source = &ctx->sources[src];

            if (!source->playing)
                continue;

            const SlAudioSpec *spec = &source->spec;

            const size_t bufferId = source->queuedBuffers[source->queuedBuffersFront];
            const SlantBuffer *buf = &ctx->buffers[bufferId];

            const size_t bytePos = source->position * source->sampleStride;

            float sampleL = GetSample(spec->dataFormat, bytePos, buf->data);
            float sampleR = GetSample(spec->dataFormat, bytePos + source->channelStride, buf->data);

            // Linear interpolation
            {
                const size_t prevPos = source->position == 0 ? 0 : ((source->position - 1) * source->sampleStride);
                const float prevSampleL = GetSample(spec->dataFormat, prevPos, buf->data);
                const float prevSampleR = GetSample(spec->dataFormat, prevPos + source->channelStride, buf->data);
                sampleL = LERP(prevSampleL, sampleL, source->finePosition);
                sampleR = LERP(prevSampleR, sampleR, source->finePosition);
            }

            buffer[i + 0] += sampleL;
            buffer[i + 1] += sampleR;

            source->finePosition += source->speedAdjust;
            // Increase the source's position by the integer value of its fine position.
            // Ensures that the position remains as accurate as possible.
            const size_t iFinePos = (size_t) source->finePosition;
            source->position += iFinePos;
            source->finePosition -= (double) iFinePos;

            if (source->position >= source->currentBufferLengthInSamples)
            {
                source->playing = false;
            }
        }
    }
}

SlResult slCreateBuffer(SlContext *context, SlBuffer *buffer)
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

SlResult slUpdateBuffer(SlContext* context, const SlBuffer buffer, const size_t dataSize, const void* data)
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

SlResult slCreateSource(SlContext *context, const SlSourceInfo *info, SlSource *source)
{
    CHECK_CONTEXT(context);
    SlantContext *ctx = (SlantContext *) context;

    if (info->type != SL_SOURCE_PCM)
        return SL_RESULT_INVALID_PARAMETER;

    SlAudioSpec spec = info->spec;

    SlantSource src;
    src.valid = true;
    src.spec = spec;
    src.speedAdjust = (double) spec.sampleRate / (double) ctx->sampleRate;
    src.channelStride = 0;

    // Set the channel stride to the size of the data format in bytes.
    // The channel stride determines how far the sampler should go to find the next channel.
    // In the case of stereo, it determines how many bytes from the left sample, is the right sample.
    switch (spec.dataFormat)
    {
        case SL_FORMAT_I8:
        case SL_FORMAT_U8:
            src.channelStride = 1;
            break;
        case SL_FORMAT_I16:
            src.channelStride = 2;
            break;
        case SL_FORMAT_I32:
        case SL_FORMAT_F32:
            src.channelStride = 4;
            break;
    }

    // The sample stride is the size in bytes of a Slant sample, which includes all channels.
    src.sampleStride = src.channelStride * spec.channels;

    src.queuedBuffers = (size_t *) malloc(INITIAL_CAPACITY * sizeof(size_t));
    src.queuedBuffersCapacity = INITIAL_CAPACITY;
    src.queuedBuffersFront = 0;
    src.queuedBuffersBack = 0;
    if (!src.queuedBuffers)
        return SL_RESULT_OUT_OF_MEMORY;

    src.currentBufferLengthInSamples = 0;

    src.playing = false;
    src.position = 0;
    src.finePosition = 0.0;

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

SlResult slSourceQueueBuffer(SlContext* context, SlSource source, SlBuffer buffer)
{
    CHECK_CONTEXT(context);
    const SlantContext *ctx = (SlantContext *) context;
    CHECK_SOURCE(ctx, source);
    CHECK_BUFFER(ctx, buffer);

    SlantSource *src = &ctx->sources[source.id];
    src->queuedBuffers[src->queuedBuffersBack] = buffer.id;
    src->queuedBuffersBack++;
    // TODO: This won't work when queueing multiple buffers.
    src->currentBufferLengthInSamples = ctx->buffers[buffer.id].dataLength / src->sampleStride;

    return SL_RESULT_OK;
}

SlResult slSourceGetState(SlContext* context, SlSource source, SlSourceState* state)
{
    CHECK_CONTEXT(context);
    const SlantContext *ctx = (SlantContext *) context;
    CHECK_SOURCE(ctx, source);

    const SlantSource *src = &ctx->sources[source.id];

    if (src->playing)
        *state = SL_STATE_PLAYING;
    else if (src->position == 0)
        *state = SL_STATE_STOPPED;
    else
        *state = SL_STATE_PAUSED;

    return SL_RESULT_OK;
}

SlResult slSourcePlay(SlContext* context, const SlSource source)
{
    CHECK_CONTEXT(context);
    const SlantContext *ctx = (SlantContext *) context;
    CHECK_SOURCE(ctx, source);
    SlantSource *src = &ctx->sources[source.id];
    src->playing = true;

    return SL_RESULT_OK;
}

SlResult slSourcePause(SlContext* context, const SlSource source)
{
    CHECK_CONTEXT(context);
    const SlantContext *ctx = (SlantContext *) context;
    CHECK_SOURCE(ctx, source);
    SlantSource *src = &ctx->sources[source.id];
    src->playing = false;

    return SL_RESULT_OK;
}

SlResult slSourceStop(SlContext* context, const SlSource source)
{
    CHECK_CONTEXT(context);
    const SlantContext *ctx = (SlantContext *) context;
    CHECK_SOURCE(ctx, source);
    SlantSource *src = &ctx->sources[source.id];
    src->playing = false;

    // Clear and reset the source.
    src->position = 0;
    src->finePosition = 0;
    src->queuedBuffersBack = 0;
    src->queuedBuffersFront = 0;

    return SL_RESULT_OK;
}
