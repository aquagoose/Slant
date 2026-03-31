#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>
#include <stdint.h>

#define ENSURE_ARGS(ExpectedArgs, GotArgs, Message) if (GotArgs - 1 < ExpectedArgs) {\
    printf("Expected %d args, got %d. "Message, ExpectedArgs, GotArgs - 1);\
    return 1;\
}

uint8_t *LoadFileToBuffer(const char *path, size_t *length);

#endif
