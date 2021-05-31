/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioListener
*/

#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

#include "ige/plugin/AudioPlugin/AudioListener.hpp"

namespace ige {
namespace plugin {
    namespace audio {

        AudioListener::AudioListener()
            : AudioListener(glm::vec3(0.0f, 0.0f, 0.0f))
        {
        }

        AudioListener::AudioListener(glm::vec3 pos)
        {
            set_position(vec);
        }

        AudioListener::~AudioListener()
        {
        }

        glm::vec3 AudioListener::position() const
        {
            return value3f(AL_POSITION);
        }

        void AudioListener::set_position(glm::vec3 pos)
        {
            set_value3f(AL_POSITION, pos);
        }

        glm::vec3 AudioListener::value3f(ALenum param) const
        {
            glm::vec3 ret;

            alGetListener3f(param, &ret.x, &ret.y, &ret.z);
            return ret;
        }

        void AudioListener::set_value3f(ALenum param, glm::vec3 value)
        {
            alListener3f(param, value.x, value.y, value.z);
        }
    }
}
}
