/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioPlugin
*/

#include "ige/plugin/AudioPlugin/AudioPlugin.hpp"
#include "ige/plugin/AudioPlugin/AudioEngine.hpp"

namespace ige {
namespace plugin {
    namespace audio {

        void AudioPlugin::plug(core::App::Builder& builder) const
        {
            builder.emplace<AudioEngine>();
        }

    }
}
}
