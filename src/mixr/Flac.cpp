#include "mixr++/Stream/Flac.h"

#include "mixr++/Stream/Flac.hpp"

using namespace mixr::Stream;

void mxStreamLoadFlac(const char* path, MxAudioStream **pAudioStream) {
    Flac* flac = new Flac(path);
    *pAudioStream = (MxAudioStream*) flac;
}