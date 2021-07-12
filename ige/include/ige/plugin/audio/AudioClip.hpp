#ifndef AUDIOCLIP_HPP_
#define AUDIOCLIP_HPP_

#include "ige/core/Task.hpp"
#include "ige/plugin/audio/AudioBuffer.hpp"
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>

namespace ige::plugin::audio {

class AudioClip {
public:
    using Handle = std::shared_ptr<AudioClip>;

    AudioClip(
        std::span<const std::int16_t> samples, std::uint32_t sample_rate,
        std::uint8_t channels);
    AudioClip(const AudioClip&) = delete;
    AudioClip& operator=(const AudioClip&) = delete;

    AudioBuffer& audio_buffer();
    const AudioBuffer& audio_buffer() const;

    static Handle load(std::string_view path);
    static core::Task<Handle> load_async(std::string);

private:
    std::uint8_t m_channel = 2;
    std::uint32_t m_sample_rate = 44100;
    AudioBuffer m_buffer;
};

}

#endif /* !AUDIOCLIP_HPP_ */
