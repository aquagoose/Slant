#pragma once

#include "AudioStream.h"
#include "../Slant.h"

#ifdef __cplusplus
extern "C" {
#endif

    Slant_C_API void mxStreamLoadVorbis(const char *path, MxAudioStream **stream);

#ifdef __cplusplus
}
#endif