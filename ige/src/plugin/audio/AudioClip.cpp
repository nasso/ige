#include "ige/plugin/audio/AudioClip.hpp"
#include "AudioEngine.hpp"
#include "ige/core/Task.hpp"
#include "ige/plugin/audio/exceptions/AudioPluginException.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <cassert>
#include <cstdint>
#include <glm/common.hpp>
#include <libnyquist/Decoders.h>
#include <limits>
#include <memory>
#include <span>
#include <string>
#include <vector>

using ige::core::Task;
using ige::plugin::audio::AudioBuffer;
using ige::plugin::audio::AudioClip;
using ige::plugin::audio::AudioPluginException;

AudioClip::AudioClip(
    std::span<const std::int16_t> samples, std::uint32_t sample_rate,
    std::uint8_t channels)
{
    assert(0 < channels && channels < 3);

    m_buffer.set_data<std::int16_t>(
        channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, samples,
        sample_rate);
    AudioEngine::get_native_exception();
}

AudioBuffer& AudioClip::audio_buffer()
{
    return m_buffer;
}

const AudioBuffer& AudioClip::audio_buffer() const
{
    return m_buffer;
}

static std::vector<std::int16_t> to_u16(std::span<const float> vec)
{
    const float SHORT_MAX_F
        = static_cast<float>(std::numeric_limits<std::int16_t>::max());

    std::vector<std::int16_t> ret;

    ret.reserve(vec.size());
    for (float v : vec) {
        // 16bits samples are passed to openAL as a signed short with 0
        // as silence
        ret.push_back(static_cast<std::int16_t>(glm::round(v * SHORT_MAX_F)));
    }

    return ret;
}

AudioClip::Handle AudioClip::load(const std::string& path)
{
    nqr::NyquistIO loader;
    nqr::AudioData audio_data;

    std::cout << "[INFO] Loading audio clip " << path << "..." << std::endl;

    loader.Load(&audio_data, path);
    auto u16_buffer = to_u16(audio_data.samples);

    std::cout << "[INFO] Loaded audio clip " << path << ":\n"
              << "Duration: " << audio_data.lengthSeconds << " seconds.\n"
              << "Sample rate: " << audio_data.sampleRate << " Hz\n"
              << "Channels: " << audio_data.channelCount << std::endl;

    return std::make_shared<AudioClip>(
        u16_buffer, static_cast<std::uint32_t>(audio_data.sampleRate),
        static_cast<std::uint8_t>(audio_data.channelCount));
}

Task<AudioClip::Handle> AudioClip::load_async(std::string path)
{
    return Task<AudioClip::Handle>::spawn(
        [path = std::move(path)] { return AudioClip::load(path); });
}
