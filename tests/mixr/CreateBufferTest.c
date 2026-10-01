#include <mixr/context.h>
#include <stdio.h>

#define CHECK(result, operation) {\
    MxResult res = result;\
    if (res != MX_RESULT_OK) {\
        printf("Mixr operation \"%s\" failed: %s\n", operation, mxResultToString(res));\
        return 1;\
    }\
}

int main(int argc, char **argv)
{
    MxContextInfo contextInfo =
    {
        .sampleRate = 44100
    };

    MxContext *context;
    CHECK(mxCreateContext(&contextInfo, &context), "Create context");

    MxBufferInfo bufferInfo = {};

    MxBuffer buffer;
    CHECK(mxCreateBuffer(context, &bufferInfo, &buffer), "Create buffer");

    uint8_t data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    CHECK(mxUpdateBuffer(context, buffer, data, 16), "Update buffer");

    CHECK(mxDestroyContext(context), "Destroy context");
    return 0;
}