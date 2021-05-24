/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioSource
*/

#include <optional>
#include <AL/al.h>
#include <AL/alc.h>

#include "ige/plugin/AudioPlugin/exceptions/AudioPluginException.hpp"
#include "ige/plugin/AudioPlugin/AudioSource.hpp"
#include "ige/plugin/AudioPlugin/AudioEngine.hpp"

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

void AudioSource::load_clip(AudioClip *clip)
{
    m_clip = clip;
    alSourcei(this->m_source, AL_PITCH, 1);
    alSourcei(this->m_source, AL_LOOPING, 0);
    alSourcef(this->m_source, AL_GAIN, 1.0f);
    alSourcei(this->m_source, AL_BUFFER, clip->get_al_buffer());
    AudioEngine::get_native_exception();
    alSourcePlay(this->m_source);
    AudioEngine::get_native_exception();
}

void AudioSource::set_position(Vector3f vec)
{
    set_property3f(AL_POSITION, vec);
}

Vector3f AudioSource::get_position()
{
    return get_property3f(AL_POSITION);
}

void AudioSource::set_velocity(Vector3f vec)
{
    set_property3f(AL_VELOCITY, vec);
}

Vector3f AudioSource::get_velocity()
{
    return get_property3f(AL_VELOCITY);
}

Vector3f AudioSource::get_property3f(ALenum alProp)
{
    Vector3f ret;

    alGetSource3f(m_source, alProp, &ret.x, &ret.y, &ret.z);
    return ret;
}

void AudioSource::set_property3f(ALenum alProp, Vector3f vec)
{
    alSource3f(m_source, alProp, vec.x, vec.y, vec.z);
}

}
}
}
