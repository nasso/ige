#ifndef AUDIOLISTENER_HPP_
#define AUDIOLISTENER_HPP_

#include <AL/al.h>
#include <AL/alc.h>
#include <glm/vec3.hpp>

namespace ige::plugin::audio {

class AudioListener {
public:
    AudioListener(glm::vec3 = { 0.0f, 0.0f, 0.0f });

    glm::vec3 position() const;
    void set_position(glm::vec3 pos);

private:
    glm::vec3 value3f(ALenum param) const;
    void set_value3f(ALenum param, glm::vec3 value);
};

}

#endif /* !AUDIOLISTENER_HPP_ */
