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

    CHECK(mxDestroyDevice(device), "Destroy device");
    return 0;
}