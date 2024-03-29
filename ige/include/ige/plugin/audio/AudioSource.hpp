#ifndef AUDIOSOURCE_HPP_
#define AUDIOSOURCE_HPP_

#include "ige/plugin/audio/AudioClip.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <glm/vec3.hpp>
#include <memory>

namespace ige::plugin::audio {

class AudioSource {
public:
    AudioSource();
    AudioSource(AudioSource&&);
    AudioSource& operator=(AudioSource&&);
    AudioSource(const AudioSource& other) = delete;
    ~AudioSource();

    void load_clip(std::shared_ptr<AudioClip> clip);

    void play();
    void pause();
    void restart();
    void stop();

    bool is_playing();

    void set_looping(bool loop = true);
    bool looping() const;

    void set_volume(float volume);
    float volume() const;

    void set_position(glm::vec3 vec);
    glm::vec3 position() const;

    void set_velocity(glm::vec3 vec);
    glm::vec3 velocity() const;

    void set_reference_distance(float distance);
    float reference_distance() const;

private:
    ALuint m_source;
    bool m_moved = false;
    std::shared_ptr<AudioClip> m_clip;

    glm::vec3 get_property3f(ALenum alProp) const;
    void set_property3f(ALenum alProp, glm::vec3 vec);

    float get_propertyf(ALenum alProp) const;
    void set_propertyf(ALenum alProp, float value);

    int get_propertyi(ALenum alProp) const;
    void set_propertyi(ALenum alProp, int value);
};

}

#endif /* !AUDIOSOURCE_HPP_ */
