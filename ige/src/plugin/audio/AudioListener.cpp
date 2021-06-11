#include "ige/plugin/audio/AudioListener.hpp"
#include <AL/al.h>
#include <glm/vec3.hpp>

using glm::vec3;
using ige::plugin::audio::AudioListener;

AudioListener::AudioListener(vec3 pos)
{
    set_position(pos);
    alListenerf(AL_GAIN, 0.5f);
}

vec3 AudioListener::position() const
{
    return value3f(AL_POSITION);
}

void AudioListener::set_position(vec3 pos)
{
    set_value3f(AL_POSITION, pos);
}

vec3 AudioListener::value3f(ALenum param) const
{
    vec3 ret;

    alGetListener3f(param, &ret.x, &ret.y, &ret.z);
    return ret;
}

void AudioListener::set_value3f(ALenum param, vec3 value)
{
    alListener3f(param, value.x, value.y, value.z);
}
