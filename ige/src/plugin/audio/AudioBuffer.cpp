#include "ige/plugin/audio/AudioBuffer.hpp"
#include "ige/plugin/audio/AudioEngine.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>

using ige::plugin::audio::AudioBuffer;
using ige::plugin::audio::AudioEngine;

AudioBuffer::AudioBuffer()
{
    alGenBuffers(1, &(this->m_buffer));
    AudioEngine::get_native_exception();
}

AudioBuffer::AudioBuffer(AudioBuffer&& other)
{
    this->m_buffer = other.m_buffer;
    other.m_moved = true;
}

AudioBuffer::~AudioBuffer()
{
    if (!m_moved)
        alDeleteBuffers(1, &this->m_buffer);
}

unsigned int AudioBuffer::get_internal_handle() const
{
    return m_buffer;
}

AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other)
{
    this->m_buffer = other.m_buffer;
    other.m_moved = true;
    return *this;
}
