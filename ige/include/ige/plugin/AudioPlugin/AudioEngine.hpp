#ifndef KAL_HPP_
#define KAL_HPP_

#include <AL/al.h>
#include <AL/alc.h>

namespace ige {
namespace audio {

    class AudioEngine {
    public:
        AudioEngine();
        ~AudioEngine();

    protected:
    private:
        ALCdevice* device = nullptr;
    };

}
}

#endif /* !KAL_HPP_ */
