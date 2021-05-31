/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioListener
*/

#ifndef AUDIOLISTENER_HPP_
#define AUDIOLISTENER_HPP_

#include <AL/al.h>
#include <AL/alc.h>
#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

namespace ige {
namespace plugin {
    namespace audio {
        class AudioListener {
        public:
            AudioListener();
            AudioListener(glm::vec3);
            ~AudioListener();

            glm::vec3 position() const;
            void set_position(glm::vec3 pos);

        protected:
        private:
            glm::vec3 value3f(ALenum param) const;
            void set_value3f(ALenum param, glm::vec3 value);
        };
    }
}
}

#endif /* !AUDIOLISTENER_HPP_ */
