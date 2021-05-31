/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioSource
*/

#ifndef AUDIOSOURCE_HPP_
#define AUDIOSOURCE_HPP_

#include <AL/al.h>
#include <AL/alc.h>
#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58
#include <optional>

#include "ige/plugin/AudioPlugin/AudioClip.hpp"
#include "ige/plugin/AudioPlugin/AudioEngine.hpp"

namespace ige {
namespace plugin {
    namespace audio {

        class AudioSource {
        public:
            AudioSource();
            ~AudioSource();

            void load_clip(AudioClip*);

            void play();
            void pause();
            void resume();
            void restart();

            bool is_playing();

            void set_position(glm::vec3 vec);
            glm::vec3 get_position();

            void set_velocity(glm::vec3 vec);
            glm::vec3 get_velocity();

        protected:
        private:
            ALuint m_source;
            AudioClip* m_clip;

            glm::vec3 get_property3f(ALenum alProp) const;
            void set_property3f(ALenum alProp, glm::vec3 vec);

            float get_propertyf(ALenum alProp) const;
            void set_propertyf(ALenum alProp, float vec);
        };

    }
}
}

#endif /* !AUDIOSOURCE_HPP_ */
