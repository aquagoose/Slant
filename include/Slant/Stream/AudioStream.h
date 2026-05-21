#pragma once

#include "../Slant.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct SlAudioStream SlAudioStream;

    SLANT_C_API SlAudioFormat slStreamGetFormat(SlAudioStream *stream);

    SLANT_C_API size_t slStreamGetBuffer(SlAudioStream *stream, uint8_t *buffer, size_t bufferLength);

    SLANT_C_API void slStreamRestart(SlAudioStream *stream);
    SLANT_C_API void slStreamSeekToSample(SlAudioStream *stream, size_t sample);

    SLANT_C_API size_t slStreamGetPositionInSamples(SlAudioStream *stream);

    SLANT_C_API size_t slStreamGetLengthInSamples(SlAudioStream *stream);
    SLANT_C_API void slStreamGetPCM(SlAudioStream *stream, uint8_t *data, size_t *dataLength);

    SLANT_C_API void slDestroyStream(SlAudioStream *stream);

#ifdef __cplusplus
}
#endif
