#pragma once

#include <cstdint>

#include "Common.h"

namespace mixr {

    class MIXR_CPP_API AudioBuffer {
    private:
        size_t _id;
        Impl* _impl;

    public:
        AudioBuffer(size_t id, Impl* impl);
        ~AudioBuffer();

        inline size_t ID() {
            return _id;
        }

        void Update(uint8_t* data, size_t dataLength);
    };

}
