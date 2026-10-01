#ifndef MIXR_CONTEXT_H
#define MIXR_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"

#include <stdint.h>
#include <stddef.h>

typedef struct MxContext MxContext;
typedef size_t MxBuffer;

typedef struct MxContextInfo
{
    uint32_t sampleRate;
} MxContextInfo;

typedef struct MxBufferInfo
{

} MxBufferInfo;

MxResult mxCreateContext(const MxContextInfo *info, MxContext **context);
MxResult mxDestroyContext(MxContext *context);

MxResult mxCreateBuffer(MxContext *context, const MxBufferInfo *info, MxBuffer *buffer);
//MxResult mxDestroyBuffer(MxContext *context, MxBuffer buffer);

#ifdef __cplusplus
}
#endif
#endif //MIXR_CONTEXT_H
