#include "ige.hpp"
#include <chrono>
#include <iostream>
#include <optional>
#include <stdio.h>
#include <thread>

using namespace std::literals::chrono_literals;
using ige::asset::Material;
using ige::asset::Mesh;
using ige::asset::Texture;
using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::Schedule;
using ige::plugin::audio::AudioClip;
using ige::plugin::audio::AudioEngine;
using ige::plugin::audio::AudioListener;
using ige::plugin::audio::AudioPlugin;
using ige::plugin::audio::AudioSource;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;

class RootState : public State {

    void on_start(App& app) override
    {
        std::shared_ptr<AudioClip> clip(
            new AudioClip("./assets/waves_mono.ogg"));

        // Warning: Only mono sound files support 3D Spatialization
        auto demo_ent = app.world().create_entity();
        demo_ent.emplace_component<AudioSource>();
        demo_ent.add_component<Transform>(
            Transform::from_pos(glm::vec3(500.0f, 0.0f, 0.0f)));
        demo_ent.get_component<AudioSource>()->load_clip(clip);
        demo_ent.get_component<AudioSource>()->play();

        // Warning: A world can only contain one AudioListener !
        auto listener = app.world().create_entity();
        listener.emplace_component<Transform>();
        listener.emplace_component<AudioListener>(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    void on_update(App& app) override
    {
        static size_t count = 0;
        auto listeners = app.world().query<AudioListener, Transform>();
        auto source = app.world().query<AudioSource, Transform>();

        if (!std::get<1>(source[0]).is_playing()) {
            app.quit();
        }
        for (auto& [entity, listener, xform] : listeners) {
            xform.translate(glm::vec3(1, 0, 0));
            std::cout << "New listener position:"
                      << " X=" << xform.translation().x
                      << " Y=" << xform.translation().y
                      << " Z=" << xform.translation().z << std::endl;
        }

        // This is just to slow the process down a bit, don't use that in a real
        // use case
        std::this_thread::sleep_for(10ms);
    }
};

int main()
{
    std::string holder;

    std::cout
        << "[Audio Example] This example loads a test OGG file and performs 3D "
           "Spatialization using a source and a listener. You should be able "
           "to hear the sound go from right to left (You need headphones). The "
           "switch should happen at X=500.\n\nPress [Enter] when you're ready"
        << std::endl;
    std::getline(std::cin, holder);
    App::Builder()
        .add_plugin(TransformPlugin {})
        .add_plugin(AudioPlugin {})
        .run<RootState>();
    return (0);
}
