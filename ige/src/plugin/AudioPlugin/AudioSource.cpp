/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioSource
*/

#include <AL/al.h>
#include <AL/alc.h>
#include <optional>

#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

#include "ige/plugin/AudioPlugin/AudioEngine.hpp"
#include "ige/plugin/AudioPlugin/AudioSource.hpp"
#include "ige/plugin/AudioPlugin/exceptions/AudioPluginException.hpp"

namespace ige {
namespace plugin {
    namespace audio {

        AudioSource::AudioSource()
        {
            alGenSources(1, &m_source);
        }

        AudioSource::~AudioSource()
        {
            alDeleteSources(1, &m_source);
        }

        bool AudioSource::is_playing()
        {
            ALint is_playing;

            alGetSourcei(m_source, AL_SOURCE_STATE, &is_playing);
            return (is_playing == AL_PLAYING);
        }

        void AudioSource::load_clip(std::shared_ptr<AudioClip> clip)
        {
            m_clip = clip;
            alSourcei(this->m_source, AL_LOOPING, 0);
            AudioEngine::get_native_exception();
            alSourcef(this->m_source, AL_GAIN, 1.0f);
            AudioEngine::get_native_exception();
            alSourcef(this->m_source, AL_ROLLOFF_FACTOR, 1.0f);
            AudioEngine::get_native_exception();
            alSourcei(this->m_source, AL_BUFFER, clip->get_al_buffer());
            AudioEngine::get_native_exception();
            alSourcei(this->m_source, AL_MAX_DISTANCE, 500);
            AudioEngine::get_native_exception();
            alSourcei(this->m_source, AL_REFERENCE_DISTANCE, 250);
            AudioEngine::get_native_exception();
        }

        void AudioSource::play()
        {
            alSourcePlay(this->m_source);
            AudioEngine::get_native_exception();
        }

        void AudioSource::pause()
        {
            alSourcePause(this->m_source);
            AudioEngine::get_native_exception();
        }

        void AudioSource::restart()
        {
            alSourceRewind(this->m_source);
            AudioEngine::get_native_exception();
        }

        void AudioSource::set_position(glm::vec3 vec)
        {
            set_property3f(AL_POSITION, vec);
        }

        glm::vec3 AudioSource::position() const
        {
            return get_property3f(AL_POSITION);
        }

        void AudioSource::set_velocity(glm::vec3 vec)
        {
            set_property3f(AL_VELOCITY, vec);
        }

        glm::vec3 AudioSource::velocity() const
        {
            return get_property3f(AL_VELOCITY);
        }

        void AudioSource::set_reference_distance(float distance)
        {
            set_propertyf(AL_REFERENCE_DISTANCE, distance);
        }

        float AudioSource::reference_distance() const
        {
            return get_propertyf(AL_REFERENCE_DISTANCE);
        }

        glm::vec3 AudioSource::get_property3f(ALenum alProp) const
        {
            glm::vec3 ret;

            alGetSource3f(m_source, alProp, &ret.x, &ret.y, &ret.z);
            return ret;
        }

        void AudioSource::set_property3f(ALenum alProp, glm::vec3 vec)
        {
            alSource3f(m_source, alProp, vec.x, vec.y, vec.z);
        }

        float AudioSource::get_propertyf(ALenum alProp) const
        {
            float ret;

            alGetSourcef(m_source, alProp, &ret);
            return ret;
        }

        void AudioSource::set_propertyf(ALenum alProp, float value)
        {
            alSourcef(m_source, alProp, value);
        }
    }
}
}
