#include "mixr/Stream/AudioStream.h"
#include "mixr/Stream/Wav.h"
#include "mixr++/Stream/Wav.h"

using namespace mixr::Stream;

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