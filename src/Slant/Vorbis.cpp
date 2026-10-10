#include "Slant/Stream/AudioStream.h"
#include "Slant/Stream/Vorbis.h"
#include "Slant++/Stream/Vorbis.h"

using namespace sl::Stream;

void slStreamLoadVorbis(const char* path, SlAudioStream **pAudioStream) {
    Vorbis* vorbis = new Vorbis(path);
    *pAudioStream = (SlAudioStream*) vorbis;
}