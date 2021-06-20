#include "ige.hpp"
#include <glm/common.hpp>
#include <iostream>
#include <optional>

using ige::core::App;
using ige::core::State;
using ige::core::Task;
using ige::ecs::EntityId;
using ige::plugin::audio::AudioClip;
using ige::plugin::audio::AudioListener;
using ige::plugin::audio::AudioPlugin;
using ige::plugin::audio::AudioSource;
using ige::plugin::time::Time;
using ige::plugin::time::TimePlugin;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;

class DemoState : public State {
    std::optional<EntityId> source_entity;
    AudioClip::Handle clip;

public:
    DemoState(AudioClip::Handle clip)
        : clip(clip)
    {
    }

    void on_start(App& app) override
    {
        // Warning: Only mono sound files support 3D Spatialization
        source_entity = app.world().create_entity(Transform {});

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
        auto xform = app.world().get_component<Transform>(*source_entity);

        if (!source->is_playing()) {
            app.quit();
        }

        float t = app.world().get<Time>()->now_seconds();
        float distance = 10.0f + glm::sin(2.0f * t) * 5.0f;

        xform->set_translation({
            glm::cos(0.5f * t) * distance,
            0.0f,
            glm::sin(0.5f * t) * distance,
        });
    }
};

class LoadingState : public State {
    Task<AudioClip::Handle> clip_task;

public:
    void on_start(App& app) override
    {
        clip_task = AudioClip::load_async("./assets/waves_mono.ogg");
        std::cout << "Loading audio clip..." << std::endl;
    }

    void on_update(App& app) override
    {
        if (clip_task) {
            std::cout << "Audio clip loaded! Starting demo..." << std::endl;
            app.state_machine().switch_to<DemoState>(*clip_task);
        }
    }
};

int main()
{
    App::Builder()
        .add_plugin(TimePlugin {})
        .add_plugin(TransformPlugin {})
        .add_plugin(AudioPlugin {})
        .run<LoadingState>();
}
