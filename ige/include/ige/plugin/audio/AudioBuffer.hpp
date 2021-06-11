#ifndef AUDIOBUFFER_HPP_
#define AUDIOBUFFER_HPP_

#include <AL/al.h>
#include <AL/alc.h>
#include <cstddef>
#include <span>

namespace ige::plugin::audio {

class AudioBuffer {
public:
    AudioBuffer();
    AudioBuffer(const AudioBuffer&) = delete;
    AudioBuffer& operator=(const AudioBuffer&) = delete;
    AudioBuffer(AudioBuffer&&);
    AudioBuffer& operator=(AudioBuffer&&);
    ~AudioBuffer();

    template <typename T>
    void
    set_data(ALenum format, std::span<const T> data, std::size_t sample_rate)
    {
        alBufferData(
            m_buffer, format, data.data(),
            static_cast<ALsizei>(data.size_bytes()),
            static_cast<ALsizei>(sample_rate));
    }

    ALuint get_internal_handle() const;

private:
    ALuint m_buffer;
    bool m_moved = false;
};

}

#endif /* !AUDIOBUFFER_HPP_ */
