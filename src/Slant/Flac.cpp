#include "Slant/Stream/AudioStream.h"
#include "Slant/Stream/Flac.h"
#include "Slant++/Stream/Flac.h"

using namespace Slant::Stream;

void slStreamLoadFlac(const char* path, SlAudioStream **pAudioStream) {
    Flac* flac = new Flac(path);
    *pAudioStream = (SlAudioStream*) flac;
}