#include "ige/plugin/AudioPlugin/AudioEngine.hpp"
#include "ige/plugin/AudioPlugin/exceptions/AudioPluginException.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <string.h>
#include <vector>

namespace ige {
namespace plugin {
    namespace audio {

        AudioEngine::AudioEngine()
            : AudioEngine(NULL)
        {
            alGetError();
        }

        AudioEngine::AudioEngine(const std::string& deviceName)
            : AudioEngine(deviceName.c_str())
        {
        }

        AudioEngine::AudioEngine(const char* deviceName)
        {
            if ((this->m_device = alcOpenDevice(deviceName)) == NULL) {
                AudioEngine::get_native_exception();
            }
            if ((this->m_context = alcCreateContext(this->m_device, NULL))
                == NULL) {
                AudioEngine::get_native_exception();
            }
            alcMakeContextCurrent(this->m_context);
            alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
            AudioEngine::get_native_exception();
        }

        AudioEngine::~AudioEngine()
        {
            alcMakeContextCurrent(NULL);
            alcDestroyContext(this->m_context);
            alcCloseDevice(this->m_device);
        }

        std::vector<std::string> AudioEngine::get_available_devices()
        {
            std::vector<std::string> ret = {};
            const ALchar* devices_raw
                = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);

            if (!devices_raw)
                AudioEngine::get_native_exception();
            while (strlen(devices_raw) > 0) {
                ret.push_back(devices_raw);
                devices_raw += strlen(devices_raw) + 1;
            }
            return ret;
        }

        void AudioEngine::get_native_exception()
        {
            ALenum err = alGetError();
            std::string ret = "";
            const char* holder = NULL;

            if (err != AL_NO_ERROR) {
                holder = alGetString(err);
                if (holder)
                    ret = holder;
                throw AudioPluginException("Native openAL exception: " + ret);
            }
        }
    }
}
}
