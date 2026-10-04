#include <mixr/device.h>
#include <stdio.h>

#define CHECK(result, operation) {\
    printf(operation"\n");\
    MxResult res = result;\
    if (res != MX_RESULT_OK) {\
        printf("Mixr operation \"%s\" failed: %s\n", operation, mxResultToString(res));\
        return 1;\
    }\
}

int main(int argc, char **argv)
{
    MxDeviceInfo deviceInfo =
    {
        .sampleRate = 44100
    };

    MxDevice *device;
    CHECK(mxCreateDevice(&deviceInfo, &device), "Create device");

    MxBufferInfo bufferInfo = {};
    MxBuffer buffer;
    CHECK(mxCreateBuffer(device, &bufferInfo, &buffer), "Create buffer");

    uint8_t data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    CHECK(mxUpdateBuffer(device, buffer, data, 16), "Update buffer");

    MxAudioFormat format =
    {
        .type = MX_DATA_TYPE_U8,
        .sampleRate = 4,
        .channels = 1
    };

    MxSourceInfo sourceInfo =
    {
        .flags = 0,
        .format = format
    };

    MxSource source;
    CHECK(mxCreateSource(device, &sourceInfo, &source), "Create source");

    CHECK(mxDestroyDevice(device), "Destroy device");
    return 0;
}