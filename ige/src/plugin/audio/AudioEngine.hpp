#ifndef AUDIO_ENGINE_HPP_
#define AUDIO_ENGINE_HPP_

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <vector>

class AudioEngine {
public:
    AudioEngine();
    AudioEngine(const std::string&);
    AudioEngine(const char*);
    ~AudioEngine();

    std::vector<std::string> get_available_devices();
    static void get_native_exception();

private:
    ALCdevice* m_device = nullptr;
    ALCcontext* m_context = nullptr;
};

#endif /* !AUDIO_ENGINE_HPP_ */
