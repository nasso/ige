/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioClip
*/

#include <AL/al.h>
#include <AL/alc.h>
#include <libnyquist/Decoders.h>
#include <string>
#include <vector>

#include "ige/plugin/AudioPlugin/AudioClip.hpp"
#include "ige/plugin/AudioPlugin/AudioEngine.hpp"
#include "ige/plugin/AudioPlugin/exceptions/AudioPluginException.hpp"

namespace ige {
namespace plugin {
    namespace audio {

        static std::vector<char> to_al_vector8(std::vector<float> vec)
        {
            std::vector<char> ret = {};

            for (float v : vec) {
                ret.push_back(abs(floor(v * 255)));
            }
            return ret;
        }

        static std::vector<signed short> to_al_vector16(std::vector<float> vec)
        {
            std::vector<signed short> ret = {};

            for (float v : vec) {
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
                      << "Sample Rate   : " << this->m_audio_data.sampleRate
                      << "Hz\n"
                      << "Channel Count : " << this->m_audio_data.channelCount
                      << "\n"
                      << "Frame Size    : " << this->m_audio_data.frameSize
                      << std::endl;
            alGenBuffers(1, &(this->m_buffer));
            AudioEngine::get_native_exception();
            auto sample_mode = this->find_sample_mode(this->m_audio_data);
            if (sample_mode == AL_FORMAT_MONO8
                || sample_mode == AL_FORMAT_STEREO8) {
                auto alvec = to_al_vector8(this->m_audio_data.samples);
                alBufferData(
                    this->m_buffer, this->find_sample_mode(this->m_audio_data),
                    alvec.data(), alvec.size(), this->m_audio_data.sampleRate);
            } else {
                auto alvec = to_al_vector16(this->m_audio_data.samples);
                alBufferData(
                    this->m_buffer, this->find_sample_mode(this->m_audio_data),
                    alvec.data(), alvec.size() * sizeof(signed short),
                    this->m_audio_data.sampleRate);
            }
            AudioEngine::get_native_exception();
        }

        AudioClip::AudioClip(const AudioClip& other)
        {
            *this = other;
        }

        AudioClip::~AudioClip()
        {
            alDeleteBuffers(1, &this->m_buffer);
        }

        AudioClip& AudioClip::operator=(const AudioClip& other)
        {
            this->m_audio_data = other.m_audio_data;
            this->m_buffer = other.m_buffer;
            return *this;
        }

        ALenum AudioClip::find_sample_mode(const nqr::AudioData& data)
        {
            ALenum sample_mode = AL_FORMAT_MONO8;

            if (data.channelCount == 1) {
                sample_mode
                    = (data.frameSize <= 8 ? AL_FORMAT_MONO8
                                           : AL_FORMAT_MONO16);
            } else if (data.channelCount == 2) {
                sample_mode
                    = (data.frameSize <= 8 ? AL_FORMAT_STEREO8
                                           : AL_FORMAT_STEREO16);
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

        ALuint AudioClip::get_al_buffer()
        {
            return this->m_buffer;
        }

    }
}
}
