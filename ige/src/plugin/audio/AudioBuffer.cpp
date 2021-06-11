#include "ige/plugin/audio/AudioBuffer.hpp"
#include "AudioEngine.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <utility>

using ige::plugin::audio::AudioBuffer;

AudioBuffer::AudioBuffer()
{
    alGenBuffers(1, &m_buffer);
    AudioEngine::get_native_exception();
}

AudioBuffer::AudioBuffer(AudioBuffer&& other)
    : m_buffer(other.m_buffer)
{
    other.m_moved = true;
}

AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other)
{
    if (!m_moved) {
        alDeleteBuffers(1, &m_buffer);
    }

    m_buffer = other.m_buffer;
    other.m_moved = true;
    m_moved = false;
    return *this;
}

AudioBuffer::~AudioBuffer()
{
    if (!m_moved) {
        alDeleteBuffers(1, &m_buffer);
    }
}

ALuint AudioBuffer::get_internal_handle() const
{
    return m_buffer;
}
