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

    CHECK(mxDestroyContext(context), "Destroy context");
    return 0;
}