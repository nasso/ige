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
using ige::plugin::Transform;
using ige::plugin::TransformPlugin;
using ige::plugin::audio::AudioClip;
using ige::plugin::audio::AudioEngine;
using ige::plugin::audio::AudioPlugin;
using ige::plugin::audio::AudioSource;

class RootState : public State {

    void on_start(App& app) override
    {
        auto engine = app.world().get<AudioEngine>().value().get();

        std::cout << "Available audio devices: " << std::endl;
        for (std::string& device : engine.get_available_devices()) {
            std::cout << "    - " << device << std::endl;
        }

        auto demo_ent = app.world().create_entity();
        demo_ent.emplace_component<AudioSource>();
        demo_ent.emplace_component<Transform>();
    }

    void on_update(App& app) override
    {

        auto sources = app.world().query<Transform>();

        for (auto& [entity, xform] : sources) { }
    }
};

int main()
{
    AudioSource source;
    AudioClip clip("../resources/test.ogg");

    source.load_clip(&clip);
    while (source.is_playing()) { }

    App::Builder()
        .add_plugin(TransformPlugin {})
        .add_plugin(AudioPlugin {})
        .run<RootState>();

    return (0);
}
