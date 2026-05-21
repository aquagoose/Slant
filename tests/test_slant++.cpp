#include <Slant++/Slant.h>
#include <Slant++/Stream/Wav.h>
#include <Slant++/Stream/Flac.h>
#include <Slant++/Stream/Vorbis.h>
#include <Slant++/Stream/Mp3.h>

#include <thread>
#include <iostream>

using namespace Slant;

struct CallbackData {
    std::unique_ptr<Stream::AudioStream> Stream;
    AudioSource* Source;
    std::vector<uint8_t> Buffer;
    std::vector<std::unique_ptr<AudioBuffer>> Buffers;
    int CurrentBuffer;
};

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Please enter filename as argument" << std::endl;
        return 1;
    }

    auto stream = std::make_unique<Stream::Mp3>(argv[1]);
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

    int value = 0;

    while (source->State() != SourceState::Stopped)
        {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << cbData->Stream->PositionInSamples() << std::endl;
    }

    return 0;
}