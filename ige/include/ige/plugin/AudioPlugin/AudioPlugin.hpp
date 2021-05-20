#ifndef AUDIOPLUGIN_HPP_
#define AUDIOPLUGIN_HPP_

#include "ige.hpp"
#include "ige/plugin/AudioPlugin/AudioEngine.hpp"

namespace ige {
namespace plugin {
namespace audio {

class AudioPlugin : public core::App::Plugin {
    public:
        void plug(core::App::Builder&) const override;
};

}
}
}

#endif /* !AUDIOPLUGIN_HPP_ */
