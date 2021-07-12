#include "igepch.hpp"

#include "AudioEngine.hpp"
#include "ige/plugin/audio/exceptions/AudioPluginException.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <cstring>

using ige::plugin::audio::AudioPluginException;

AudioEngine::AudioEngine()
    : AudioEngine(nullptr)
{
    alGetError();
}

AudioEngine::AudioEngine(const std::string& deviceName)
    : AudioEngine(deviceName.c_str())
{
}

AudioEngine::AudioEngine(const char* deviceName)
{
    if ((m_device = alcOpenDevice(deviceName)) == nullptr) {
        AudioEngine::get_native_exception();
    }
    if ((m_context = alcCreateContext(m_device, nullptr)) == nullptr) {
        AudioEngine::get_native_exception();
    }
    alcMakeContextCurrent(m_context);
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    AudioEngine::get_native_exception();
}

AudioEngine::AudioEngine(AudioEngine&& engine)
{
    *this = std::move(engine);
}

AudioEngine& AudioEngine::operator=(AudioEngine&& other)
{
    if (!this->m_moved) {
        alcDestroyContext(this->m_context);
        alcCloseDevice(this->m_device);
    }
    this->m_context = other.m_context;
    this->m_device = other.m_device;
    other.m_moved = true;
    this->m_moved = false;
    return *this;
}

AudioEngine::~AudioEngine()
{
    if (!m_moved) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_context);
        alcCloseDevice(m_device);
    }
}

std::vector<std::string> AudioEngine::get_available_devices()
{
    std::vector<std::string> ret;
    const ALchar* devices_raw
        = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);

    if (!devices_raw) {
        AudioEngine::get_native_exception();
    }

    while (*devices_raw) {
        ret.push_back(devices_raw);
        devices_raw += std::strlen(devices_raw) + 1;
    }

    return ret;
}

void AudioEngine::get_native_exception()
{
    ALenum err = alGetError();
    std::string ret;
    const char* holder = nullptr;

    if (err != AL_NO_ERROR) {
        holder = alGetString(err);

        if (holder) {
            ret = holder;
        }

        throw AudioPluginException("Native openAL exception: " + ret);
    }
}
