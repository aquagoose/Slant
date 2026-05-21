#pragma once

#include "AudioStream.h"
#include "../Slant.h"

#ifdef __cplusplus
extern "C" {
#endif

    Slant_C_API void mxStreamLoadFlac(const char* path, MxAudioStream **pAudioStream);

#ifdef __cplusplus
}
#endif