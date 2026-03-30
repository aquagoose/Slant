#include "Slant/Context.h"

#include <stdlib.h>

typedef struct
{
    float masterVolume;
} SlantContext;

SlResult slCreateContext(SlContextInfo *info, SlContext **context)
{
    SlantContext *ctx = (SlantContext *) malloc(sizeof(SlantContext));
    if (!ctx)
    {
        *context = NULL;
        return SL_RESULT_OUT_OF_MEMORY;
    }

    ctx->masterVolume = 1.0f;

    *context = (SlContext *) ctx;
    return SL_RESULT_OK;
}

void slDestroyContext(SlContext *context)
{
    SlantContext *ctx = (SlantContext *) context;
    free(ctx);
}

void slContextMixStereoF32(SlContext *context, uint32_t sampleRate, float *buffer, size_t bufferLength)
{
    SlantContext *ctx = (SlantContext *) context;
}
