#include <stdlib.h>
#include <unistd.h>
#include <mixr/mixr.h>
#include <mixr/Stream/Wav.h>
#include <mixr/Stream/Flac.h>
#include <mixr/Stream/Vorbis.h>
#include <mixr/Stream/Mp3.h>

int main(int argc, char **argv)
{
    MxAudioStream* stream;
    mxStreamLoadVorbis(argv[1], &stream);

    MxAudioFormat format = mxStreamGetFormat(stream);

    size_t dataLength;
    mxStreamGetPCM(stream, NULL, &dataLength);
    uint8_t* data = malloc(dataLength);
    mxStreamGetPCM(stream, data, &dataLength);

    MxDevice* device;
    MxContext* context;
    mxCreateDevice(48000, &device);
    mxDeviceGetContext(device, &context);

    //mxContextSetMasterVolume(context, 0.1f);

    MxSourceDescription description;
    description.Type = MX_SOURCE_TYPE_PCM;
    description.Format = format;

    mxDestroyStream(stream);

    MxAudioBuffer buffer = mxContextCreateBuffer(context, data, dataLength);

    MxAudioSource source = mxContextCreateSource(context, &description);
    mxSourceSubmitBuffer(context, source, buffer);

    //mxSourceSetSpeed(context, source, 2.0);
    //mxSourceSetVolume(context, source, 0.5f);
    mxSourceSetLooping(context, source, true);
    //mxSourceSetPanning(context, source, -1.0f);

    mxSourcePlay(context, source);
    
    while (mxSourceGetState(context, source) != MX_SOURCE_STATE_STOPPED)
    {
        usleep(1000000);
    }
    
    mxDestroyDevice(device);
    
    return 0;
}