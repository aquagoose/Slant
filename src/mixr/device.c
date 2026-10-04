#include "mixr/device.h"

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

typedef struct MixrDevice
{
    uint32_t sampleRate;

    MixrBuffer *buffers;
    size_t buffersLength;
    size_t buffersCapacity;

    MixrSource *sources;
    size_t sourcesLength;
    size_t sourcesCapacity;
} MixrDevice;

MxResult mxCreateDevice(const MxDeviceInfo *info, MxDevice **device)
{
    if (!info)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    // sample rate cannot be 0
    if (info->sampleRate == 0)
        return MX_RESULT_ERROR_INVALID_PARAMETER;

    MixrDevice *dev = (MixrDevice *) malloc(sizeof(MixrDevice));
    if (!dev)
        return MX_RESULT_ERROR_OUT_OF_MEMORY;

    dev->sampleRate = info->sampleRate;

    dev->buffersCapacity = 16;
    dev->buffersLength = 0;
    dev->buffers = (MixrBuffer *) malloc(dev->buffersCapacity * sizeof(MixrBuffer));

    dev->sourcesCapacity = 16;
    dev->sourcesLength = 0;
    dev->sources = (MixrSource *) malloc(dev->sourcesCapacity * sizeof(MixrSource));

    *device = (MxDevice *) dev;
    return MX_RESULT_OK;
}

MxResult mxDestroyDevice(MxDevice *device)
{
    if (!device)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    MixrDevice *dev = (MixrDevice *) device;

    for (size_t i = 0; i < dev->buffersLength; i++)
        free(dev->buffers[i].data);

    free(dev->sources);
    free(dev->buffers);
    free(dev);

    return MX_RESULT_OK;
}

MxResult mxCreateBuffer(MxDevice *device, const MxBufferInfo *info, MxBuffer *buffer)
{
    if (!device)
        return MX_RESULT_ERROR_NULL_PARAMETER;
    if (!info)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    MixrDevice *dev = (MixrDevice *) device;

    // resize buffers list if needed
    if (dev->buffersLength + 1 >= dev->buffersCapacity)
    {
        dev->buffersCapacity <<= 1;
        MixrBuffer *buffers = (MixrBuffer *) realloc(dev->buffers, dev->buffersCapacity * sizeof(MixrBuffer));
        if (!buffers)
            return MX_RESULT_ERROR_OUT_OF_MEMORY;
        dev->buffers = buffers;
    }

    size_t bufferID = dev->buffersLength++;
    *buffer = (MxBuffer) bufferID;

    MixrBuffer buf;
    buf.valid = true;
    buf.data = NULL;
    buf.dataLength = 0;
    buf.dataCapacity = 0;
    dev->buffers[bufferID] = buf;

    return MX_RESULT_OK;
}

/*MxResult mxDestroyBuffer(MxDevice *device, MxBuffer buffer)
{
    if (!device)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    MixrDevice dev
}*/

MxResult mxUpdateBuffer(MxDevice *device, MxBuffer buffer, void *data, size_t dataSize)
{
    if (!device)
        return MX_RESULT_ERROR_NULL_PARAMETER;
    if (!data && dataSize > 0)
        return MX_RESULT_ERROR_INVALID_PARAMETER;

    MixrDevice *dev = (MixrDevice *) device;
    if (buffer >= dev->buffersLength || !dev->buffers[buffer].valid)
        return MX_RESULT_ERROR_INVALID_BUFFER;

    MixrBuffer *buf = &dev->buffers[buffer];

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

MxResult mxCreateSource(MxDevice *device, const MxSourceInfo *info, MxSource *source)
{
    if (!device)
        return MX_RESULT_ERROR_NULL_PARAMETER;
    if (!info)
        return MX_RESULT_ERROR_NULL_PARAMETER;

    if (info->format.channels == 0 || info->format.channels > 2)
        return MX_RESULT_ERROR_INVALID_PARAMETER;
    if (info->format.sampleRate == 0)
        return MX_RESULT_ERROR_INVALID_PARAMETER;
    if (info->format.type < 0 || info->format.type > MX_DATA_TYPE_F32)
        return MX_RESULT_ERROR_INVALID_PARAMETER;

    MixrDevice *dev = (MixrDevice *) device;

    // resize sources if necessary
    if (dev->sourcesLength + 1 >= dev->sourcesCapacity)
    {
        dev->sourcesCapacity <<= 1;
        MixrSource *sources = (MixrSource *) realloc(dev->sources, dev->sourcesCapacity * sizeof(MixrSource));
        if (!sources)
            return MX_RESULT_ERROR_OUT_OF_MEMORY;
        dev->sources = sources;
    }

    size_t sourceID = dev->sourcesLength++;
    *source = (MxSource) sourceID;
    MixrSource src;
    src.valid = true;
    src.format = info->format;
    dev->sources[sourceID] = src;

    return MX_RESULT_OK;
}
