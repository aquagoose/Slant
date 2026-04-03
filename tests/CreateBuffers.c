#include "Utils/Utils.h"

#include <Slant/Slant.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    ENSURE_ARGS(1, argc, "Please provide the path to a raw PCM file.")

    SlContextInfo contextInfo;
    contextInfo.sampleRate = 44100;

    SlContext *context;
    SlResult result = slCreateContext(&contextInfo, &context);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to create context! %s\n", slResultToString(result));
        return 1;
    }

    SlBuffer buffer;
    result = slCreateBuffer(context, &buffer);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to create buffer! %s\n", slResultToString(result));
        return 1;
    }

    size_t dataLength;
    uint8_t *data = LoadFileToBuffer(argv[1], &dataLength);
    if (!data)
    {
        printf("File '%s' could not be opened.\n", argv[1]);
        return 1;
    }

    result = slUpdateBuffer(context, buffer, dataLength, data);
    free(data);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to update buffer! %s\n", slResultToString(result));
        return 1;
    }

    slDestroyContext(context);
    return 0;
}
