#include "Slant/Stream/AudioStream.h"
#include "Slant/Stream/Wav.h"
#include "Slant++/Stream/Wav.h"

using namespace Slant::Stream;

void slStreamLoadWav(const char* path, SlAudioStream **pAudioStream) {
    Wav* wav = new Wav(path);
    *pAudioStream = (SlAudioStream*) wav;
}

bool slWavIsADPCM(SlAudioStream *stream) {
    Wav* wav = (Wav*) stream;
    return wav->IsADPCM();
}

SlADPCMInfo slWavGetADPCMInfo(SlAudioStream *stream) {
    Wav* wav = (Wav*) stream;

    ADPCMInfo info = wav->ADPCMInfo();
    auto slInfo = reinterpret_cast<SlADPCMInfo*>(&info);

    return *slInfo;
}