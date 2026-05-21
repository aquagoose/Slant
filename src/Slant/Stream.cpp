#include "Slant/Stream/AudioStream.h"
#include "Slant++/Stream/AudioStream.h"

using namespace Slant;
using namespace Slant::Stream;

SlAudioFormat slStreamGetFormat(SlAudioStream *stream) {
    AudioStream* aStream = (AudioStream*) stream;

    AudioFormat format = aStream->Format();

    return {
       /* .DataType = */ (SlDataType) format.DataType,
       /* .SampleRate = */format.SampleRate,
       /* .Channels = */ format.Channels
    };
}

size_t slStreamGetBuffer(SlAudioStream *stream, uint8_t *buffer, size_t bufferLength) {
    AudioStream* aStream = (AudioStream*) stream;

    return aStream->GetBuffer(buffer, bufferLength);
}

void slStreamRestart(SlAudioStream *stream) {
    AudioStream* aStream = (AudioStream*) stream;

    aStream->Restart();
}

void slStreamSeekToSample(SlAudioStream *stream, size_t sample) {
    AudioStream* aStream = (AudioStream*) stream;

    aStream->SeekToSample(sample);
}

size_t slStreamGetPositionInSamples(SlAudioStream *stream) {
    AudioStream* aStream = (AudioStream*) stream;

    return aStream->PositionInSamples();
}

size_t slStreamGetLengthInSamples(SlAudioStream *stream) {
    AudioStream* aStream = (AudioStream*) stream;

    return aStream->LengthInSamples();
}

void slStreamGetPCM(SlAudioStream *stream, uint8_t *data, size_t *dataLength) {
    AudioStream* aStream = (AudioStream*) stream;
    auto format = aStream->Format();

    *dataLength = aStream->LengthInSamples() * format.BytesPerSample() * format.Channels;

    if (data) {
        auto pcmData = aStream->GetPCM();
        auto dataPtr = pcmData.data();
        std::copy(dataPtr, dataPtr + *dataLength, data);
    }
}

void slDestroyStream(SlAudioStream *stream) {
    AudioStream* aStream = (AudioStream*) stream;
    delete aStream;
}