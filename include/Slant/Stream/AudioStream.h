#pragma once

#include "../Slant.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct MxAudioStream MxAudioStream;

    Slant_C_API MxAudioFormat mxStreamGetFormat(MxAudioStream *stream);

    Slant_C_API size_t mxStreamGetBuffer(MxAudioStream *stream, uint8_t *buffer, size_t bufferLength);

    Slant_C_API void mxStreamRestart(MxAudioStream *stream);
    Slant_C_API void mxStreamSeekToSample(MxAudioStream *stream, size_t sample);

    Slant_C_API size_t mxStreamGetPositionInSamples(MxAudioStream *stream);

    Slant_C_API size_t mxStreamGetLengthInSamples(MxAudioStream *stream);
    Slant_C_API void mxStreamGetPCM(MxAudioStream *stream, uint8_t *data, size_t *dataLength);

    Slant_C_API void mxDestroyStream(MxAudioStream *stream);

#ifdef __cplusplus
}
#endif
