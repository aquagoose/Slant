#include "Slant/Stream/AudioStream.h"
#include "Slant/Stream/Flac.h"
#include "Slant++/Stream/Flac.h"

using namespace Slant::Stream;

void mxStreamLoadFlac(const char* path, MxAudioStream **pAudioStream) {
    Flac* flac = new Flac(path);
    *pAudioStream = (MxAudioStream*) flac;
}