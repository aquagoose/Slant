#include <stdlib.h>
#include <unistd.h>
#include <Slant/Slant.h>
#include <Slant/Stream/Wav.h>
#include <Slant/Stream/Flac.h>
#include <Slant/Stream/Vorbis.h>
#include <Slant/Stream/Mp3.h>

int main(int argc, char **argv)
{
    SlAudioStream* stream;
    slStreamLoadMp3(argv[1], &stream);

    SlAudioFormat format = slStreamGetFormat(stream);

    size_t dataLength;
    slStreamGetPCM(stream, NULL, &dataLength);
    uint8_t* data = malloc(dataLength);
    slStreamGetPCM(stream, data, &dataLength);

    SlDevice* device;
    SlContext* context;
    slCreateDevice(48000, &device);
    slDeviceGetContext(device, &context);

    //slContextSetMasterVolume(context, 0.1f);

    SlSourceDescription description;
    description.Type = SL_SOURCE_TYPE_PCM;
    description.Format = format;

    slDestroyStream(stream);

    SlAudioBuffer buffer = slContextCreateBuffer(context, data, dataLength);

    SlAudioSource source = slContextCreateSource(context, &description);
    slSourceSubmitBuffer(context, source, buffer);

    //slSourceSetSpeed(context, source, 2.0);
    //slSourceSetVolume(context, source, 0.5f);
    slSourceSetLooping(context, source, true);
    //slSourceSetPanning(context, source, -1.0f);

    slSourcePlay(context, source);
    
    while (slSourceGetState(context, source) != SL_SOURCE_STATE_STOPPED)
    {
        usleep(1000000);
    }
    
    slDestroyDevice(device);
    
    return 0;
}