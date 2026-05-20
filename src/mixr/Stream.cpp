#include "mixr/Stream/AudioStream.h"
#include "mixr++/Stream/AudioStream.h"

using namespace mixr;
using namespace mixr::Stream;

MxAudioFormat mxStreamGetFormat(MxAudioStream *stream) {
    AudioStream* aStream = (AudioStream*) stream;

    AudioFormat format = aStream->Format();

    return {
       /* .DataType = */ (MxDataType) format.DataType,
       /* .SampleRate = */format.SampleRate,
       /* .Channels = */ format.Channels
    };
}

size_t mxStreamGetBuffer(MxAudioStream *stream, uint8_t *buffer, size_t bufferLength) {
    AudioStream* aStream = (AudioStream*) stream;

    return aStream->GetBuffer(buffer, bufferLength);
}

void mxStreamRestart(MxAudioStream *stream) {
    AudioStream* aStream = (AudioStream*) stream;

    aStream->Restart();
}

void mxStreamSeekToSample(MxAudioStream *stream, size_t sample) {
    AudioStream* aStream = (AudioStream*) stream;

    aStream->SeekToSample(sample);
}

size_t mxStreamGetPositionInSamples(MxAudioStream *stream) {
    AudioStream* aStream = (AudioStream*) stream;

    return aStream->PositionInSamples();
}

size_t mxStreamGetLengthInSamples(MxAudioStream *stream) {
    AudioStream* aStream = (AudioStream*) stream;

    return aStream->LengthInSamples();
}

void mxStreamGetPCM(MxAudioStream *stream, uint8_t *data, size_t *dataLength) {
    AudioStream* aStream = (AudioStream*) stream;
    auto format = aStream->Format();

    *dataLength = aStream->LengthInSamples() * format.BytesPerSample() * format.Channels;

    if (data) {
        auto pcmData = aStream->GetPCM();
        auto dataPtr = pcmData.data();
        std::copy(dataPtr, dataPtr + *dataLength, data);
    }
}

void mxDestroyStream(MxAudioStream *stream) {
    AudioStream* aStream = (AudioStream*) stream;
    delete aStream;
}