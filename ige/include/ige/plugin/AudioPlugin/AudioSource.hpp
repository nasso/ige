/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioSource
*/

#ifndef AUDIOSOURCE_HPP_
#define AUDIOSOURCE_HPP_

#include <optional>
#include <AL/al.h>
#include <AL/alc.h>

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

        void set_position(Vector3f vec);
        Vector3f get_position();

        void set_velocity(Vector3f vec);
        Vector3f get_velocity();

    protected:
    private:
        ALuint m_source;
        AudioClip *m_clip;

        Vector3f get_property3f(ALenum alProp);
        void set_property3f(ALenum alProp, Vector3f vec);
};

}
}
}

#endif /* !AUDIOSOURCE_HPP_ */
