#pragma once

#include "AudioStream.h"
#include "../Slant.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum {
        MX_ADPCM_TYPE_IMA
    } MxADPCMType;

    typedef struct {
        MxADPCMType Type;
        size_t ChunkSize;
    } MxADPCMInfo;

    Slant_C_API void mxStreamLoadWav(const char* path, MxAudioStream **pAudioStream);

    Slant_C_API bool mxWavIsADPCM(MxAudioStream *stream);
    Slant_C_API MxADPCMInfo mxWavGetADPCMInfo(MxAudioStream *stream);

#ifdef __cplusplus
}
#endif