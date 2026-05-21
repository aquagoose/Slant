#include <iostream>
#include "Slant++/Utils/ADPCM.h"

inline int Clamp(int value, int min, int max) {
    return value <= min ? min : value >= max ? max : value;
}

namespace Slant::Utils::ADPCM {
    int IndexTable[] = {
            -1, -1, -1, -1, 2, 4, 6, 8,
            -1, -1, -1, -1, 2, 4, 6, 8
    };

    int StepTable[] = {
            7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
            19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
            50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
            130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
            337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
            876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
            2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
            5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
            15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
    };

    std::vector<uint8_t> DecodeIMA(uint8_t* data, size_t dataLength, bool stereo, size_t chunkSize) {
        std::vector<uint8_t> result(dataLength * 4);

        size_t currentPos = 0;
        for (size_t c = 0; c < dataLength; c += chunkSize) {
            DecodeIMAChunk(data + c, chunkSize, result.data() + (currentPos * 4), stereo);
            currentPos += chunkSize - (stereo ? 8 : 4);
        }

        return result;
    }

    void DecodeIMAChunk(uint8_t* inBuffer, size_t chunkSize, uint8_t* outBuffer, bool stereo) {
        int channels = stereo ? 2 : 1;

        int predictor[2];
        int stepIndex[2];
        int step[2];

        size_t newIndex[2];
        newIndex[0] = 0;
        if (stereo)
            newIndex[1] = 2;

        int chunkOffset = 4;
        predictor[0] = (int16_t) (inBuffer[0] | inBuffer[1] << 8);
        stepIndex[0] = inBuffer[2];
        step[0] = StepTable[stepIndex[0]];

        //std::cout << predictor[0] << std::endl;

        if (stereo) {
            chunkOffset = 8;
            predictor[1] = (int16_t) (inBuffer[4] | inBuffer[5] << 8);
            stepIndex[1] = inBuffer[6];
            step[1] = StepTable[stepIndex[1]];
        }

        for (size_t i = chunkOffset; i < chunkSize; i++) {
            uint8_t b = inBuffer[i];

            int channel = 0;
            if (stereo) {
                // TODO: Can condense this into a single statement and remove the ternary, I think.
                bool isRightChannel = (i % 8) / 4 == 1;
                channel = isRightChannel ? 1 : 0;
            }

            for (int n = 0; n < 2; n++) {
                uint8_t nibble = (uint8_t) (n == 1 ? (b >> 4) & 0xF : (b & 0xF));

                int sign = nibble & 8;
                int delta = nibble & 7;
                int diff = step[channel] >> 3;
                if ((delta & 4) == 4)
                    diff += step[channel];
                if ((delta & 2) == 2)
                    diff += step[channel] >> 1;
                if ((delta & 1) == 1)
                    diff += step[channel] >> 2;
                if (sign)
                    predictor[channel] -= diff;
                else
                    predictor[channel] += diff;

                predictor[channel] = Clamp(predictor[channel], INT16_MIN, INT16_MAX);

                stepIndex[channel] += IndexTable[nibble];
                stepIndex[channel] = Clamp(stepIndex[channel], 0, 88);
                step[channel] = StepTable[stepIndex[channel]];

                int16_t currentPredictor = (int16_t) predictor[channel];

                outBuffer[newIndex[channel]] = (uint8_t) (currentPredictor & 0xFF);
                outBuffer[newIndex[channel] + 1] = (uint8_t) (currentPredictor >> 8);

                newIndex[channel] += channels * 2;
            }
        }
    }
}