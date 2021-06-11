#ifndef AUDIOPLUGIN_HPP_
#define AUDIOPLUGIN_HPP_

#include "audio/AudioClip.hpp"
#include "audio/AudioListener.hpp"
#include "audio/AudioSource.hpp"
#include "ige/core/App.hpp"

namespace ige::plugin::audio {

class AudioPlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* !AUDIOPLUGIN_HPP_ */
