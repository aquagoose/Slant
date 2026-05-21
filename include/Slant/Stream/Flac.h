#pragma once

#include "AudioStream.h"
#include "../Slant.h"

#ifdef __cplusplus
extern "C" {
#endif

    SLANT_C_API void slStreamLoadFlac(const char* path, SlAudioStream **pAudioStream);

#ifdef __cplusplus
}
#endif