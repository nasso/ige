#include "igepch.hpp"

#include "audio/AudioEngine.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/AudioPlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/audio/AudioListener.hpp"
#include "ige/plugin/audio/AudioSource.hpp"

using glm::vec3;
using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::audio::AudioListener;
using ige::plugin::audio::AudioPlugin;
using ige::plugin::audio::AudioSource;
using ige::plugin::transform::Transform;

static void update_positions_system(World& world)
{
    for (auto [entity, source] : world.query<AudioSource>()) {
        auto xform = world.get_component<Transform>(entity);

        if (xform) {
            source.set_position(xform->translation());
        } else {
            source.set_position(vec3(0.0f, 0.0f, 0.0f));
        }
    }

    for (auto [entity, listener] : world.query<AudioListener>()) {
        auto xform = world.get_component<Transform>(entity);

        if (xform) {
            listener.set_position(xform->translation());
        } else {
            listener.set_position(vec3(0.0f, 0.0f, 0.0f));
        }
    }
}

void AudioPlugin::plug(App::Builder& builder) const
{
    builder.emplace<AudioEngine>();
    builder.add_system(System::from(update_positions_system));
    builder.add_cleanup_system(
        System::from([](World& world) { world.remove<AudioEngine>(); }));
}
