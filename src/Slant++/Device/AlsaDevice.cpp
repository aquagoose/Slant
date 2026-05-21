#include "AlsaDevice.h"

#include <iostream>
#include <stdexcept>
#include <thread>

#define SL_ALSA_CHECK(Res, Msg) {\
    int error = Res;\
    if (error < 0) {\
        throw std::runtime_error(Msg": " + std::string(snd_strerror(error)));\
    }\
}

namespace Slant::Device
{
    void AlsaDevice::CallbackThread(AlsaDevice* device)
    {
        auto buffer = new uint8_t[device->_bufferSize];

        while (device->_active)
        {
            device->GetBuffer(buffer, device->_bufferSize);

            auto frames = snd_pcm_writei(device->_device, buffer, device->_bufferSizeFrames);

            if (frames < 0)
                frames = snd_pcm_recover(device->_device, frames, 0);
            if (frames < 0)
                throw std::runtime_error("OOPS: " + std::string(snd_strerror(frames)));

            if (frames > 0 && frames < device->_bufferSizeFrames)
                std::cout << "Err" << std::endl;
        }

        delete buffer;
    }

    AlsaDevice::AlsaDevice(const uint32_t sampleRate) : AudioDevice(sampleRate)
    {
        SL_ALSA_CHECK(snd_pcm_open(&_device, "default", SND_PCM_STREAM_PLAYBACK, 0), "Failed to open device");
        //SL_ALSA_CHECK(snd_pcm_set_params(_device, SND_PCM_FORMAT_FLOAT, SND_PCM_ACCESS_RW_INTERLEAVED, 2, sampleRate, true, 500000), "Failed to set params");

        snd_pcm_hw_params_t* params;
        snd_pcm_hw_params_alloca(&params);

        SL_ALSA_CHECK(snd_pcm_hw_params_any(_device, params), "Failed to set any params");
        SL_ALSA_CHECK(snd_pcm_hw_params_set_rate_resample(_device, params, true), "Failed to enable resampling");
        SL_ALSA_CHECK(snd_pcm_hw_params_set_access(_device, params, SND_PCM_ACCESS_RW_INTERLEAVED), "Failed to set interleaved mode");
        SL_ALSA_CHECK(snd_pcm_hw_params_set_format(_device, params, SND_PCM_FORMAT_FLOAT), "Failed to set float format");
        SL_ALSA_CHECK(snd_pcm_hw_params_set_rate(_device, params, sampleRate, 0), "Failed to set sample rate");
        SL_ALSA_CHECK(snd_pcm_hw_params_set_channels(_device, params, 2), "Failed to set channels");
        SL_ALSA_CHECK(snd_pcm_hw_params_set_period_size(_device, params, 256, 0), "Failed to set period size");
        SL_ALSA_CHECK(snd_pcm_hw_params_set_periods(_device, params, 2, 0), "Failed to set num periods");

        SL_ALSA_CHECK(snd_pcm_hw_params(_device, params), "Failed to set hw params");

        snd_pcm_uframes_t size;
        SL_ALSA_CHECK(snd_pcm_hw_params_get_buffer_size(params, &size), "Failed to get buffer size");

        _bufferSizeFrames = size;
        _bufferSize = snd_pcm_frames_to_bytes(_device, _bufferSizeFrames);

        std::cout << "Frames: " << size << ", Buffer Size: " << _bufferSize << std::endl;

        _active = true;
        std::thread thread(CallbackThread, this);
        thread.detach();
    }

    AlsaDevice::~AlsaDevice()
    {
        std::cout << "Destroy" << std::endl;
        _active = false;
        snd_pcm_close(_device);
    }
}
