/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioClip
*/

#ifndef AUDIOCLIP_HPP_
#define AUDIOCLIP_HPP_

#include <AL/al.h>
#include <AL/alc.h>
#include <libnyquist/Decoders.h>

namespace ige {
namespace plugin {
    namespace audio {

        class AudioClip {
        public:
            AudioClip(const std::string& path);
            AudioClip(const AudioClip& other) = delete;
            AudioClip(AudioClip&&);
            ~AudioClip();

            std::vector<float>& get_samples();
            AudioBuffer& get_audio_buffer();

            AudioClip& operator=(AudioClip&& other);

        protected:
        private:
            nqr::AudioData m_audio_data;
            AudioBuffer m_buffer;
            bool m_moved;

            ALenum find_sample_mode(const nqr::AudioData&);
        };

    }
}
}

#endif /* !AUDIOCLIP_HPP_ */
