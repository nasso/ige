#include "igepch.hpp"

#include "ige/asset/AnimationClip.hpp"
#include "ige/asset/Skeleton.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/AnimationPlugin.hpp"
#include "ige/plugin/TimePlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include <chrono>

using glm::mat4;
using ige::asset::AnimationClip;
using ige::asset::Skeleton;
using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::animation::AnimationPlugin;
using ige::plugin::animation::AnimationTrack;
using ige::plugin::animation::Animator;
using ige::plugin::animation::SkeletonPose;
using ige::plugin::time::Time;
using ige::plugin::transform::Transform;
using std::optional;
using std::chrono::duration;
using std::chrono::duration_cast;

static void compute_joint_pose(
    std::size_t idx, std::size_t frame, SkeletonPose& pose,
    const AnimationTrack::Channel& channel)
{
    const auto& joint_info = pose.skeleton->joints[idx];
    const auto& joint_anim = channel.clip->joints[idx];

    auto& joint_pose = pose.joint_pose[idx];

    // update the joint pose to the current frame
    joint_pose.pos = joint_anim.pos_sampler[frame];
    joint_pose.rotation = joint_anim.rotation_sampler[frame];
    joint_pose.scale = joint_anim.scale_sampler[frame];

    // compute global transform
    auto& mat = pose.global_pose[idx];

    if (joint_info.parent) {
        // TODO(perf): ideally, a child should always come *after* its
        // parent so that its safe to assume the parent's transform is
        // already computed here
        compute_joint_pose(*joint_info.parent, frame, pose, channel);

        mat = pose.global_pose[*joint_info.parent];
    } else {
        mat = mat4(1.0f);
    }

    mat = glm::translate(mat, joint_pose.pos);
    mat *= glm::mat4_cast(joint_pose.rotation);
    mat = glm::scale(mat, joint_pose.scale);
}

static void reset_pose(World& world, const AnimationTrack& track)
{
    for (const auto& channel : track.channels) {
        auto pose = world.get_component<SkeletonPose>(channel.target);

        if (!pose) {
            return;
        }

        std::size_t joint_count = channel.clip->joints.size();

        pose->joint_pose.resize(joint_count);
        pose->global_pose.resize(joint_count);

        // TODO(perf): find a better way to do this? maybe cache the bind
        // matrix?
        for (std::size_t i = 0; i < joint_count; i++) {
            pose->global_pose[i]
                = glm::inverse(pose->skeleton->joints[i].inv_bind_matrix);
        }
    }
}

template <typename Duration>
static void
update_animation_track(World& world, AnimationTrack& track, Duration delta)
{
    track.current_time
        += duration_cast<AnimationTrack::Duration>(track.playback_rate * delta);

    if (track.loop && track.duration != AnimationTrack::Duration::zero()) {
        track.current_time %= track.duration;
    }

    for (auto& channel : track.channels) {
        auto pose = world.get_component<SkeletonPose>(channel.target);

        if (!pose) {
            continue;
        }

        // the current animation frame
        const std::size_t frame = static_cast<std::size_t>(
            track.current_time / channel.clip->sample_duration);

        // how many joints to animate
        // should be equal to the number of joints of the skeleton
        std::size_t joint_count = channel.clip->joints.size();

#ifdef IGE_DEBUG
        if (joint_count != pose->skeleton->joints.size()) {
            std::cerr
                << "[WARN] Number of joints in the animation (" << joint_count
                << ") doesn't match the number of joints in the skeleton ("
                << pose->skeleton->joints.size() << ") - skipping animation."
                << std::endl;
            continue;
        }
#endif

        for (std::size_t j = 0; j < joint_count; j++) {
            compute_joint_pose(j, frame, *pose, channel);
        }
    }
}

static void update_animations(World& world)
{
    auto time = world.get<Time>();

    if (!time) {
        std::cerr << "[WARN] Time resource not found. The AnimationPlugin "
                     "depends on the TimePlugin."
                  << std::endl;
        return;
    }

    for (auto [ent, animator] : world.query<Animator>()) {
        // set all matrices to identity
        for (auto& track : animator.tracks()) {
            reset_pose(world, track);
        }

        // blend animations!
        for (auto& track : animator.tracks()) {
            // skip every track that won't participate in the calculation of the
            // final pose
            if (track.weight != 1.0f) {
                // TODO: animation blending!
                continue;
            }

            update_animation_track(
                world, track, animator.playback_rate * time->delta());
        }
    }
}

void AnimationPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System::from(update_animations));
}
