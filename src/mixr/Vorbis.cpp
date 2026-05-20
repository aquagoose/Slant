#include "mixr++/Stream/Vorbis.h"

#include "mixr++/Stream/Vorbis.hpp"

using namespace mixr::Stream;

void mxStreamLoadVorbis(const char* path, MxAudioStream **pAudioStream) {
    Vorbis* vorbis = new Vorbis(path);
    *pAudioStream = (MxAudioStream*) vorbis;
}