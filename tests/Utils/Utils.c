#include "Utils.h"

#include <stdlib.h>

uint8_t *LoadFileToBuffer(const char *path, size_t *length)
{
    FILE *file = fopen(path, "rb");
    if (!file)
        return NULL;

    fseek(file, 0, SEEK_END);
    *length = (size_t) ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *buffer = malloc(*length);
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, *length, file);
    fclose(file);

    return buffer;
}
