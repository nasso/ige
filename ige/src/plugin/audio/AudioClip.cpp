#include "ige/plugin/audio/AudioClip.hpp"
#include "ige/plugin/audio/AudioEngine.hpp"
#include "ige/plugin/audio/exceptions/AudioPluginException.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <libnyquist/Decoders.h>
#include <string>
#include <vector>

using ige::plugin::audio::AudioBuffer;
using ige::plugin::audio::AudioClip;
using ige::plugin::audio::AudioPluginException;

static std::vector<char> to_al_vector8(std::vector<float> vec)
{
    std::vector<char> ret = {};

    for (float v : vec) {
        // 8bits samples are passed to openAL as an unsigned byte with
        // 128 as silence
        ret.push_back(abs(floor(v * 255)));
    }
    return ret;
}

static std::vector<signed short> to_al_vector16(std::vector<float> vec)
{
    std::vector<signed short> ret = {};

    for (float v : vec) {
        // 16bits samples are passed to openAL as a signed short with 0
        // as silence
        ret.push_back(floor(v * 32767));
    }
    return ret;
}

AudioClip::AudioClip(const std::string& path)
{
    nqr::NyquistIO loader;

    alGetError();
    loader.Load(&this->m_audio_data, path);
    std::cerr << "[Audio Plugin] Loaded sound file:\n"
              << "Duration      : " << this->m_audio_data.lengthSeconds
              << " seconds.\n"
              << "Sample Rate   : " << this->m_audio_data.sampleRate << "Hz\n"
              << "Channel Count : " << this->m_audio_data.channelCount << "\n"
              << "Frame Size    : " << this->m_audio_data.frameSize
              << std::endl;
    auto sample_mode = this->find_sample_mode(this->m_audio_data);
    if (sample_mode == AL_FORMAT_MONO8 || sample_mode == AL_FORMAT_STEREO8) {
        auto alvec = to_al_vector8(this->m_audio_data.samples);

        this->m_buffer.set_data(
            sample_mode, alvec, this->m_audio_data.sampleRate);
    } else {
        auto alvec = to_al_vector16(this->m_audio_data.samples);

        this->m_buffer.set_data(
            sample_mode, alvec, this->m_audio_data.sampleRate);
    }
    AudioEngine::get_native_exception();
}

AudioClip::AudioClip(AudioClip&& source)
{
    source.m_moved = true;
    this->m_audio_data = source.m_audio_data;
    this->m_buffer = std::move(source.m_buffer);
}

AudioClip::~AudioClip()
{
}

AudioClip& AudioClip::operator=(AudioClip&& source)
{
    source.m_moved = true;
    this->m_buffer = std::move(source.m_buffer);
    this->m_audio_data = source.m_audio_data;
    return *this;
}

ALenum AudioClip::find_sample_mode(const nqr::AudioData& data)
{
    ALenum sample_mode = AL_FORMAT_MONO8;

    if (data.channelCount == 1) {
        sample_mode
            = (data.frameSize <= 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16);
    } else if (data.channelCount == 2) {
        sample_mode
            = (data.frameSize <= 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16);
    } else {
        throw AudioPluginException(
            "Invalid channel count (3+ Channels audio clip are not yet "
            "supported.)");
    }
    return (sample_mode);
}

std::vector<float>& AudioClip::get_samples()
{
    return this->m_audio_data.samples;
}

AudioBuffer& AudioClip::get_audio_buffer()
{
    return m_buffer;
}
