#pragma once

#include <cstdint>
#include <vector>

// https://gist.github.com/aquagoose/526a2d6bc829f39a01d29dc2de0b8146
namespace mixr::Utils::ADPCM {
    std::vector<uint8_t> DecodeIMA(uint8_t* data, size_t dataLength, bool stereo, size_t chunkSize);

    void DecodeIMAChunk(uint8_t* inBuffer, size_t chunkSize, uint8_t* outBuffer, bool stereo);
}