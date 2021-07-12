#include "igepch.hpp"

#include "AudioEngine.hpp"
#include "ige/core/Task.hpp"
#include "ige/plugin/audio/AudioClip.hpp"
#include "ige/plugin/audio/exceptions/AudioPluginException.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <cassert>
#include <cstdint>
#include <limits>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

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

// decode a vorbis audio file using vorbisfile
AudioClip::Handle AudioClip::load(std::string_view path)
{
    std::cout << "[INFO] Loading audio clip " << path << "..." << std::endl;

    OggVorbis_File vf;

    if (ov_fopen(path.data(), &vf) != 0) {
        throw AudioPluginException("could not open file");
    }

    vorbis_info* vi = ov_info(&vf, -1);
    if (vi == nullptr) {
        throw AudioPluginException("could not get vorbis info");
    }

    const std::uint32_t sample_rate = vi->rate;
    const std::uint8_t channels = vi->channels;
    const float duration = ov_time_total(&vf, -1);

    std::vector<std::int16_t> samples(ov_pcm_total(&vf, -1) * channels);

    std::uint32_t samples_read = 0;
    while (samples_read < samples.size()) {
        char* buf = reinterpret_cast<char*>(samples.data() + samples_read);
        const std::uint32_t len
            = static_cast<std::uint32_t>(samples.size() - samples_read);
        const std::int32_t bytes_read
            = ov_read(&vf, buf, len, 0, 2, 1, nullptr);
        if (bytes_read == 0) {
            break;
        } else if (bytes_read == OV_EBADLINK) {
            throw AudioPluginException("error decoding vorbis file");
        }

        samples_read += bytes_read / sizeof(std::int16_t);
    }

    ov_clear(&vf);

    std::cout << "[INFO] Loaded audio clip " << path << ":\n"
              << "Duration: " << duration << " seconds.\n"
              << "Sample rate: " << sample_rate << " Hz\n"
              << "Channels: " << channels << std::endl;

    return std::make_shared<AudioClip>(samples, sample_rate, channels);
}

Task<AudioClip::Handle> AudioClip::load_async(std::string path)
{
    return Task<AudioClip::Handle>::spawn(
        [path = std::move(path)]() -> AudioClip::Handle {
            try {
                return AudioClip::load(path);
            } catch (const AudioPluginException& e) {
                std::cerr << "[ERROR] Couldn't load " << path << ": "
                          << e.what() << std::endl;
                return nullptr;
            }
        });
}
