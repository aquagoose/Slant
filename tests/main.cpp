#include <mixr/mixr.hpp>
#include <mixr/Stream/Wav.hpp>
#include <mixr/Stream/Flac.hpp>
#include <mixr/Stream/Vorbis.hpp>
#include <mixr/Stream/Mp3.hpp>

#include <mixr/mixr.h>
#include <mixr/Stream/Wav.h>
#include <mixr/Stream/Flac.h>
#include <mixr/Stream/Vorbis.h>
#include <mixr/Stream/Mp3.h>

#include <thread>
#include <iostream>

using namespace mixr;

struct CallbackData {
    std::unique_ptr<Stream::AudioStream> Stream;
    AudioSource* Source;
    std::vector<uint8_t> Buffer;
    std::vector<std::unique_ptr<AudioBuffer>> Buffers;
    int CurrentBuffer;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Please enter filename as argument" << std::endl;
        return 1;
    }

    auto stream = std::make_unique<Stream::Wav>(argv[1]);
    auto format = stream->Format();
    //auto data = wav.GetPCM();

    std::cout << stream->LengthInSamples() / format.SampleRate << std::endl;

    auto device = AudioDevice::Create(48000);
    Context* context = device->Context();

    SourceDescription description {
        SourceType::PCM,
        format
    };

    //if (wav.IsADPCM()) {
    //    description.Type = SourceType::ADPCM,
    //    description.ADPCM = { .ChunkSize = wav.ADPCMInfo().ChunkSize };
    //}

    std::vector<uint8_t> buffer;
    buffer.resize(48000);

    stream->GetBuffer(buffer.data(), buffer.size());
    auto buffer1 = context->CreateBuffer(buffer.data(), buffer.size());

    stream->GetBuffer(buffer.data(), buffer.size());
    auto buffer2 = context->CreateBuffer(buffer.data(), buffer.size());

    auto source = context->CreateSource(description);
    source->SubmitBuffer(buffer1.get());
    source->SubmitBuffer(buffer2.get());

    std::vector<std::unique_ptr<AudioBuffer>> buffers;
    buffers.push_back(std::move(buffer1));
    buffers.push_back(std::move(buffer2));

    auto cbData = std::make_unique<CallbackData>(CallbackData {
        std::move(stream),
        source.get(),
        std::move(buffer),
        std::move(buffers),
        0
    });

    source->SetStateChangedCallback([](SourceState state, void* userData) -> void {
        switch (state) {
            case SourceState::Stopped:
                std::cout << "Stopped" << std::endl;
                break;
            case SourceState::Paused:
                std::cout << "Paused" << std::endl;
                break;
            case SourceState::Playing:
                std::cout << "Playing" << std::endl;
                break;
        }
    }, nullptr);

    source->SetBufferFinishedCallback([](void* userData) -> void {
        auto cbData = (CallbackData*) userData;

        auto size = cbData->Stream->GetBuffer(cbData->Buffer.data(), cbData->Buffer.size());
        //std::cout << "Request Buffer: " << size << " bytes returned" << std::endl;

        /*if (size < cbData->Buffer.size()) {
            std::cout << "Restart" << std::endl;
            cbData->Stream->Restart();
        }*/

        if (size == 0) {
            return;
        }

        cbData->Buffers[cbData->CurrentBuffer]->Update(cbData->Buffer.data(), size);
        cbData->Source->SubmitBuffer(cbData->Buffers[cbData->CurrentBuffer].get());

        cbData->CurrentBuffer++;
        if (cbData->CurrentBuffer >= cbData->Buffers.size())
            cbData->CurrentBuffer = 0;

    }, cbData.get());

    //source->SetSpeed(8);
    //source->SetPanning(1.0);
    source->Play();

    /*MxAudioStream* stream;
    mxStreamLoadMp3(argv[1], &stream);

    MxAudioFormat format = mxStreamGetFormat(stream);

    uint8_t* data;
    size_t dataLength;
    mxStreamGetPCM(stream, nullptr, &dataLength);
    data = new uint8_t[dataLength];
    mxStreamGetPCM(stream, data, &dataLength);

    MxDevice* device;
    MxContext* context;
    mxCreateSDLDevice(48000, 512, &device);
    mxDeviceGetContext(device, &context);

    //mxContextSetMasterVolume(context, 0.1f);

    MxSourceDescription description {
        MX_SOURCE_TYPE_PCM,
        format,
    };

    //if (mxWavIsADPCM(stream)) {
    //    MxADPCMInfo adpcm = mxWavGetADPCMInfo(stream);

    //    description.Type = MX_SOURCE_TYPE_ADPCM;
    //    description.ADPCM = { .ChunkSize = adpcm.ChunkSize };
    //}

    mxDestroyStream(stream);

    MxAudioBuffer buffer = mxContextCreateBuffer(context, data, dataLength);

    MxAudioSource source = mxContextCreateSource(context, &description);
    mxSourceSubmitBuffer(context, source, buffer);

    //mxSourceSetSpeed(context, source, 2.0);
    //mxSourceSetVolume(context, source, 0.5f);
    mxSourceSetLooping(context, source, true);
    //mxSourceSetPanning(context, source, -1.0f);

    mxSourcePlay(context, source);*/

    int value = 0;

    while (source->State() != SourceState::Stopped) {
    //while (mxSourceGetState(context, source) != MX_SOURCE_STATE_STOPPED) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << cbData->Stream->PositionInSamples() << std::endl;

        /*value++;

        if (value >= 5 && value < 10) {
            source->Pause();
        }

        if (value >= 10) {
            source->Play();
        }*/

        //std::cout << buffer->ID() << std::endl;
        //buffer.reset();

        //buffer = context->CreateBuffer(data.data(), data.size());
        //source->SubmitBuffer(buffer.get());
        //source->Play();

        /*std::cout << source->ID() << std::endl;
        source.reset();

        source = context->CreateSource(description);
        source->SubmitBuffer(buffer.get());
        source->Play();*/

        //source->Stop();
        //source->Play();

        /*if (test) {
            source->Play();
        } else {
            source->Pause();
        }

        test = !test;*/
    }

    //mxDestroyDevice(device);

    return 0;
}