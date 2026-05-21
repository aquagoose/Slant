#include "Slant/Stream/AudioStream.h"
#include "Slant/Stream/Mp3.h"
#include "Slant++/Stream/Mp3.h"

using namespace Slant::Stream;

void slStreamLoadMp3(const char* path, SlAudioStream **pAudioStream) {
    Mp3* mp3 = new Mp3(path);
    *pAudioStream = (SlAudioStream*) mp3;
}