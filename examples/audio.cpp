#include "ige.hpp"
#include <chrono>
#include <cstdio>
#include <iostream>
#include <optional>
#include <thread>

using ige::asset::Material;
using ige::asset::Mesh;
using ige::asset::Texture;
using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::EntityId;
using ige::ecs::Schedule;
using ige::plugin::audio::AudioClip;
using ige::plugin::audio::AudioListener;
using ige::plugin::audio::AudioPlugin;
using ige::plugin::audio::AudioSource;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;

class RootState : public State {
    std::optional<EntityId> source_entity;

public:
    void on_start(App& app) override
    {
        auto clip = AudioClip::load("./assets/waves_mono.ogg");

        // Warning: Only mono sound files support 3D Spatialization
        source_entity = app.world().create_entity(
            Transform::from_pos({ 500.0f, 0.0f, 0.0f }));

        auto& source
            = app.world().emplace_component<AudioSource>(*source_entity);
        source.load_clip(clip);
        source.play();

        // Warning: A world can only contain one AudioListener!
        auto listener
            = app.world().create_entity(AudioListener {}, Transform {});
    }

    void on_update(App& app) override
    {
        auto source = app.world().get_component<AudioSource>(*source_entity);

        if (!source->is_playing()) {
            app.quit();
        }

        auto listeners = app.world().query<AudioListener, Transform>();
        for (auto& [entity, listener, xform] : listeners) {
            xform.translate({ 1.0f, 0.0f, 0.0f });
            std::cout << "New listener position:"
                      << " X=" << xform.translation().x
                      << " Y=" << xform.translation().y
                      << " Z=" << xform.translation().z << std::endl;
        }

        // This is just to slow the process down a bit, don't use that in a real
        // use case
        {
            using namespace std::literals::chrono_literals;

            std::this_thread::sleep_for(10ms);
        }
    }
};

int main()
{
    std::cout
        << "[Audio Example] This example loads a test OGG file and performs 3D "
           "Spatialization using a source and a listener. You should be able "
           "to hear the sound go from right to left (You need headphones). The "
           "switch should happen at X=500.\n"
           "\n"
           "Press [Enter] when you're ready"
        << std::endl;

    {
        std::string str;
        std::getline(std::cin, str);
    }

    App::Builder()
        .add_plugin(TransformPlugin {})
        .add_plugin(AudioPlugin {})
        .run<RootState>();
}
