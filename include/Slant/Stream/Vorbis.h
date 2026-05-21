#pragma once

#include "AudioStream.h"
#include "../Slant.h"

#ifdef __cplusplus
extern "C" {
#endif

    SLANT_C_API void slStreamLoadVorbis(const char *path, SlAudioStream **stream);

#ifdef __cplusplus
}
#endif