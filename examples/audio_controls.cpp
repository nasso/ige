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
        auto clip = AudioClip::load("./assets/piano2.ogg");

        // Warning: Only mono sound files support 3D Spatialization
        source_entity = app.world().create_entity(Transform {});

        auto& source
            = app.world().emplace_component<AudioSource>(*source_entity);
        source.load_clip(clip);
        source.set_looping(true);

        // Warning: A world can only contain one AudioListener!
        auto listener
            = app.world().create_entity(AudioListener {}, Transform {});
    }

    void on_update(App& app) override
    {
        AudioSource* source
            = app.world().get_component<AudioSource>(*source_entity);
        std::string command;

        std::getline(std::cin, command);
        if (std::cin.eof()) {
            app.quit();
        }
        if (command == "/play") {
            source->play();
        } else if (command == "/stop") {
            source->stop();
        } else if (command == "/pause") {
            source->pause();
        } else if (command == "/restart") {
            source->restart();
        } else if (command == "/volumeup") {
            float volume = source->volume();

            if (volume + 0.1f <= 1.0f)
                source->set_volume(source->volume() + 0.1f);
        } else if (command == "/volumedown") {
            float volume = source->volume();

            if (volume - 0.1f >= 0.0f)
                source->set_volume(source->volume() - 0.1f);
        } else if (command == "/exit") {
            app.quit();
        } else {
            std::cout << "Unknown command." << std::endl;
        }
    }
};

int main()
{
    std::cout << "[Audio Controls Example] This is just a console based audio "
                 "player\n"
                 "\n"
                 "Press [Enter] when you're ready, use /exit to leave"
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
