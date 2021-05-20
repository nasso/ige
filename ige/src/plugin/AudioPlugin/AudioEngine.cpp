

#include "ige/plugin/AudioPlugin/AudioEngine.hpp"
#include "ige/plugin/AudioPlugin/exceptions/AudioPluginException.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>

namespace ige {
namespace plugin {
namespace audio {

    AudioEngine::AudioEngine()
        : AudioEngine(NULL)
    {
    }

    AudioEngine::AudioEngine(const std::string& deviceName)
        : AudioEngine(deviceName.c_str())
    {

    }

    AudioEngine::AudioEngine(const char *deviceName)
    {
        if ((this->m_device = alcOpenDevice(deviceName)) == NULL) {
            throw AudioPluginException(
                "Couldn't open device: " + get_native_exception());
        }
        if ((this->m_context = alcCreateContext(this->m_device, NULL)) == NULL) {
            throw AudioPluginException(
                "Couldn't create context: " + get_native_exception());
        }
        if (!alcMakeContextCurrent(this->m_context)) {
            throw AudioPluginException(
                "Couldn't make context current: " + get_native_exception());
        }
    }

    AudioEngine::~AudioEngine()
    {
        alcMakeContextCurrent(NULL);
        alcDestroyContext(this->m_context);
        alcCloseDevice(this->m_device);
    }
    }

    std::string AudioEngine::get_native_exception()
    {
        ALenum err = alGetError();
        std::string ret = "";

        if (err != AL_NO_ERROR) {
            ret = alGetString(err);
        }
        return ret;
    }
}
}
}
