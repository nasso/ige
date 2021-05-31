/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioPlugin
*/

#include "ige/plugin/AudioPlugin/AudioPlugin.hpp"
#include "ige/plugin/AudioPlugin/AudioEngine.hpp"

using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::Transform;
using ige::plugin::audio::AudioEngine;

namespace ige {
namespace plugin {
    namespace audio {

        static void update_positions_system(World& world)
        {
            auto sources = world.query<AudioSource>();

            for (auto& source_ent : sources) {
                auto& source = std::get<1>(source_ent);
                auto tsfm = std::get<0>(source_ent).get_component<Transform>();

                if (tsfm) {
                    source.set_position(tsfm.value().get().translation());
                } else {
                    source.set_position(glm::vec3(0.0f, 0.0f, 0.0f));
                }
            }
        }

        void AudioPlugin::plug(core::App::Builder& builder) const
        {
            builder.emplace<AudioEngine>();
            builder.add_startup_system(System(update_positions_system));
        }

    }
}
}
