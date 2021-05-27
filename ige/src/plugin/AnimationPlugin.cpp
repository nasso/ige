#include "ige/plugin/AnimationPlugin.hpp"
#include "ige/asset/AnimationClip.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TimePlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include <chrono>
#include <iostream>
#include <optional>

using ige::asset::AnimationClip;
using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::animation::AnimationPlugin;
using ige::plugin::animation::Animator;
using ige::plugin::time::Time;
using ige::plugin::transform::Transform;
using std::optional;
using std::chrono::duration;
using std::chrono::duration_cast;

namespace systems {

static void update_animations(World& world)
{
    auto time = world.get<Time>();

    if (!time) {
        std::cerr << "[WARN] Time resource not found. The AnimationPlugin "
                     "depends on the TimePlugin."
                  << std::endl;
        return;
    }

    for (auto [entity, animator] : world.query<Animator>()) {
        if (animator.current_animation) {
            const auto& anim = *animator.current_animation;

            if (!animator.paused) {
                animator.current_time += time->delta();
            }

            /*
            auto secs = duration_cast<duration<float>>(animator.current_time);
            float t = secs.count();

            for (const auto& [node_id, node_anim] : anim) {
                auto [first, last] = animator.entities(node_id);

                for (auto it = first; it != last; it++) {
                    const auto& node_entity = it->second;

                    if (world.exists(node_entity)) {
                        detail::animate_entity(
                            world, node_entity, node_anim, t);
                    } else {
                        animator.unlink(it);
                    }
                }
            }
            */
        }
    }
}

}

void AnimationPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System::from(systems::update_animations));
}
