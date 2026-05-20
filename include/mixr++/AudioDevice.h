#pragma once

#include "Context.h"

namespace mixr {

    class MIXR_CPP_API AudioDevice {
    private:
        std::unique_ptr<mixr::Context> _context;

    protected:
        explicit AudioDevice(uint32_t sampleRate);

        void GetBuffer(uint8_t* buffer, size_t dataLength);

    public:
        virtual ~AudioDevice() = default;

        Context* Context();

        static std::unique_ptr<AudioDevice> Create(uint32_t sampleRate = 44100);
    };

}
