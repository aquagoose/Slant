#include "mixr/Stream/AudioStream.h"
#include "mixr/Stream/Vorbis.h"
#include "mixr++/Stream/Vorbis.h"

using namespace mixr::Stream;

void mxStreamLoadVorbis(const char* path, MxAudioStream **pAudioStream) {
    Vorbis* vorbis = new Vorbis(path);
    *pAudioStream = (MxAudioStream*) vorbis;
}