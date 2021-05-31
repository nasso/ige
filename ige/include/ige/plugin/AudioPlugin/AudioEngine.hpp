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
            AudioEngine(const char*);

            ~AudioEngine();

            std::vector<std::string> get_available_devices();
            static void get_native_exception();

        protected:
        private:
            ALCdevice* m_device = nullptr;
            ALCcontext* m_context = nullptr;
        };

    }
}
}

#endif /* !KAL_HPP_ */
