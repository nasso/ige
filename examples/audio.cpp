#include "ige.hpp"
#include <iostream>
#include <optional>

using ige::asset::Material;
using ige::asset::Mesh;
using ige::asset::Texture;
using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::Schedule;
using ige::plugin::audio::AudioPlugin;
using ige::plugin::audio::AudioEngine;

class RootState : public State {

    void on_start(App& app) override
    {
        auto &engine = app.world().get<AudioEngine>().value().get();

        std::cout << "Available audio devices: " << std::endl;
        for (std::string &device : engine.get_available_devices()) {
            std::cout << "    - " << device << std::endl;
        }
    }

    void on_update(App& app) override
    {

    }
};

int main()
{
    std::cout << "Starting application..." << std::endl;

    App::Builder()
        .add_plugin(AudioPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
