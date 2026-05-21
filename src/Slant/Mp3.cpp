#include "Slant/Stream/AudioStream.h"
#include "Slant/Stream/Mp3.h"
#include "Slant++/Stream/Mp3.h"

using namespace Slant::Stream;

void mxStreamLoadMp3(const char* path, MxAudioStream **pAudioStream) {
    Mp3* mp3 = new Mp3(path);
    *pAudioStream = (MxAudioStream*) mp3;
}