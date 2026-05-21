#include "Slant/Stream/AudioStream.h"
#include "Slant/Stream/Wav.h"
#include "Slant++/Stream/Wav.h"

using namespace Slant::Stream;

void mxStreamLoadWav(const char* path, MxAudioStream **pAudioStream) {
    Wav* wav = new Wav(path);
    *pAudioStream = (MxAudioStream*) wav;
}

bool mxWavIsADPCM(MxAudioStream *stream) {
    Wav* wav = (Wav*) stream;
    return wav->IsADPCM();
}

MxADPCMInfo mxWavGetADPCMInfo(MxAudioStream *stream) {
    Wav* wav = (Wav*) stream;

    ADPCMInfo info = wav->ADPCMInfo();
    auto mxInfo = reinterpret_cast<MxADPCMInfo*>(&info);

    return *mxInfo;
}