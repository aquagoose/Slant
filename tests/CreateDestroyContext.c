#include <Slant/Slant.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    SlContextInfo contextInfo;

    printf("Creating context.\n");
    SlContext *context;
    SlResult result = slCreateContext(&contextInfo, &context);
    if (result != SL_RESULT_OK)
    {
        printf("Failed to create context! Error code %d\n", result);
        return 1;
    }

    printf("Destroying context.\n");
    slDestroyContext(context);

    return 0;
}