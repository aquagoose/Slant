#include <Slant/Slant.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    SlContextInfo contextInfo;
    contextInfo.sampleRate = 44100;

    printf("Creating context.\n");
    SlContext *context;
    SlResult result = slCreateContext(&contextInfo, &context);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to create context! %s\n", slResultToString(result));
        return 1;
    }

    printf("Destroying context.\n");
    slDestroyContext(context);

    return 0;
}