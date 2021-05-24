/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioClip
*/

#include <vector>
#include <string>
#include <libnyquist/Decoders.h>
#include <AL/al.h>
#include <AL/alc.h>

#include "ige/plugin/AudioPlugin/AudioClip.hpp"
#include "ige/plugin/AudioPlugin/exceptions/AudioPluginException.hpp"
#include "ige/plugin/AudioPlugin/AudioEngine.hpp"


namespace ige {
namespace plugin {
namespace audio {

std::vector<char> to_al_vector(std::vector<float> vec)
{
    std::vector<char> ret = {};

    for (float v : vec) {
        int x = v;

        ret.push_back((int)x & 0xF000);
        ret.push_back((int)x & 0x0F00);
        ret.push_back((int)x & 0x00F0);
        ret.push_back((int)x & 0x000F);
    }
    return ret;
}

AudioClip::AudioClip(const std::string &path)
{
    alGetError();
    this->m_audio_data = std::make_unique<nqr::AudioData>();
    m_nloader.Load(this->m_audio_data.get(), path);
    std::cerr << "[Audio Plugin] Loaded sound file:\n"
        << "Duration      : " << this->m_audio_data->lengthSeconds << " seconds.\n"
        << "Sample Rate   : " << this->m_audio_data->sampleRate << "Hz\n"
        << "Channel Count : " << this->m_audio_data->channelCount << "\n"
        << "Frame Size    : " << this->m_audio_data->frameSize << std::endl;
    alGenBuffers(1, &(this->m_buffer));
    AudioEngine::get_native_exception();

    auto alvec = to_al_vector(this->m_audio_data->samples);
    alBufferData(this->m_buffer, AL_FORMAT_MONO8,
        alvec.data(),
        alvec.size() * sizeof(int),
        this->m_audio_data->sampleRate);
    AudioEngine::get_native_exception();
}

AudioClip::~AudioClip()
{
    alDeleteBuffers(1, &this->m_buffer);
}

std::vector<float> &AudioClip::get_samples()
{
    return this->m_audio_data->samples;
}

ALuint AudioClip::get_al_buffer()
{
    return this->m_buffer;
}


}
}
}
