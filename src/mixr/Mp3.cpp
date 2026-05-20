#include "mixr/Stream/AudioStream.h"
#include "mixr/Stream/Mp3.h"
#include "mixr++/Stream/Mp3.h"

using namespace mixr::Stream;

void mxStreamLoadMp3(const char* path, MxAudioStream **pAudioStream) {
    Mp3* mp3 = new Mp3(path);
    *pAudioStream = (MxAudioStream*) mp3;
}