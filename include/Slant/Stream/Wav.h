#pragma once

#include "AudioStream.h"
#include "../Slant.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum {
        SL_ADPCM_TYPE_IMA
    } SlADPCMType;

    typedef struct {
        SlADPCMType Type;
        size_t ChunkSize;
    } SlADPCMInfo;

    SLANT_C_API void slStreamLoadWav(const char* path, SlAudioStream **pAudioStream);

    SLANT_C_API bool slWavIsADPCM(SlAudioStream *stream);
    SLANT_C_API SlADPCMInfo slWavGetADPCMInfo(SlAudioStream *stream);

#ifdef __cplusplus
}
#endif