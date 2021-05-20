#ifndef KAL_HPP_
#define KAL_HPP_

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <vector>

namespace ige {
namespace plugin {
namespace audio {

    class AudioEngine {
    public:
        AudioEngine();
        AudioEngine(const std::string&);
        AudioEngine(const char *);

        ~AudioEngine();


    protected:
    private:
        ALCdevice* m_device = nullptr;
        ALCcontext* m_context = nullptr;

        std::string get_native_exception();
    };

}
}
}

#endif /* !KAL_HPP_ */
