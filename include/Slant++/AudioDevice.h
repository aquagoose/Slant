#pragma once

#include "Context.h"

namespace Slant {

    class SLANT_CPP_API AudioDevice {
    private:
        std::unique_ptr<Slant::Context> _context;

    protected:
        explicit AudioDevice(uint32_t sampleRate);

        void GetBuffer(uint8_t* buffer, size_t dataLength);

    public:
        virtual ~AudioDevice() = default;

        Context* Context();

        static std::unique_ptr<AudioDevice> Create(uint32_t sampleRate = 44100);
    };

}
