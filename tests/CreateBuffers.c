#include <Slant/Slant.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Please provide a path to a file containing raw PCM data.\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file)
    {
        printf("File '%s' could not be opened.\n", argv[1]);
        return 1;
    }

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

    fseek(file, 0, SEEK_END);
    size_t fSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t* fBuffer = malloc(fSize);
    fread(fBuffer, 1, fSize, file);

    result = slContextUpdateBuffer(context, buffer, fSize, fBuffer);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to update buffer! %d\n", result);
        return 1;
    }
    free(fBuffer);

    slDestroyContext(context);
    fclose(file);
    return 0;
}
