#include <Slant/Slant.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    SlContextInfo contextInfo;
    contextInfo.sampleRate = 44100;

    SlContext *context;
    SlResult result = slCreateContext(&contextInfo, &context);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to create context! %d\n", result);
        return 1;
    }

    SlBuffer buffer;
    result = slContextCreateBuffer(context, &buffer);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to create buffer! %d\n", result);
        return 1;
    }

    slDestroyContext(context);
    return 0;
}