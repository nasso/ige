#ifndef AUDIOBUFFER_HPP_
#define AUDIOBUFFER_HPP_

#include <AL/al.h>
#include <AL/alc.h>
#include <vector>

namespace ige::plugin::audio {

class AudioBuffer {
public:
    AudioBuffer();
    AudioBuffer(const AudioBuffer&) = delete;
    AudioBuffer(AudioBuffer&&);
    ~AudioBuffer();

    template <typename T>
    void set_data(ALenum format, const std::vector<T>& data, size_t sample_rate)
    {
        alBufferData(
            this->m_buffer, format, data.data(),
            (ALsizei)(data.size() * sizeof(T)), sample_rate);
    }

    unsigned int get_internal_handle() const;

    AudioBuffer& operator=(AudioBuffer&&);

private:
    ALuint m_buffer;
    bool m_moved = false;
};

}

#endif /* !AUDIOBUFFER_HPP_ */
