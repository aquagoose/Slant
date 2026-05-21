#include "Slant/Stream/AudioStream.h"
#include "Slant/Stream/Vorbis.h"
#include "Slant++/Stream/Vorbis.h"

using namespace Slant::Stream;

void mxStreamLoadVorbis(const char* path, MxAudioStream **pAudioStream) {
    Vorbis* vorbis = new Vorbis(path);
    *pAudioStream = (MxAudioStream*) vorbis;
}