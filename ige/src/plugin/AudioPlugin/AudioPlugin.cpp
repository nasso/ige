/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioPlugin
*/

#include "ige/plugin/AudioPlugin/AudioPlugin.hpp"
#include "ige.hpp"
#include "ige/plugin/AudioPlugin/AudioEngine.hpp"

using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::audio::AudioEngine;
using ige::plugin::transform::Transform;

namespace ige {
namespace plugin {
    namespace audio {

        static void update_positions_system(World& world)
        {
            for (auto [entity, source] : world.query<AudioSource>()) {
                auto xform = world.get_component<Transform>(entity);

                if (xform) {
                    source.set_position(xform->translation());
                } else {
                    source.set_position(glm::vec3(0.0f, 0.0f, 0.0f));
                }
            }

            for (auto [entity, listener] : world.query<AudioListener>()) {
                auto xform = world.get_component<Transform>(entity);

                if (xform) {
                    listener.set_position(xform->translation());
                } else {
                    listener.set_position(glm::vec3(0.0f, 0.0f, 0.0f));
                }
            }
        }

        void AudioPlugin::plug(core::App::Builder& builder) const
        {
            builder.emplace<AudioEngine>();
            builder.add_system(System(update_positions_system));
        }

    }
}
}
