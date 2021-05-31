#include "ige.hpp"
#include <iostream>
#include <optional>
#include <stdio.h>

using ige::asset::Material;
using ige::asset::Mesh;
using ige::asset::Texture;
using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::Schedule;
using ige::plugin::audio::AudioClip;
using ige::plugin::audio::AudioEngine;
using ige::plugin::audio::AudioPlugin;
using ige::plugin::audio::AudioSource;

class RootState : public State {

    void on_start(App& app) override
    {
        try {
            auto& engine = app.world().get<AudioEngine>().value().get();

        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }

    void on_update(App& app) override
    {
    }
};

int main()
{
    AudioEngine engine;
    AudioSource source;
    AudioClip clip("../resources/test.ogg");

    std::cout << "Available audio devices: " << std::endl;
    for (std::string& device : engine.get_available_devices()) {
        std::cout << "    - " << device << std::endl;
    }
    source.load_clip(&clip);
    while (source.is_playing()) { }
    return (0);
}
