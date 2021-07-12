#include "igepch.hpp"

#include "ige/asset/Skeleton.hpp"
#include "ige/plugin/AnimationPlugin.hpp"

using ige::asset::Skeleton;
using ige::plugin::animation::SkeletonPose;

SkeletonPose::SkeletonPose(Skeleton::Handle skeleton)
    : skeleton(skeleton)
    , joint_pose(skeleton->joints.size())
    , global_pose(skeleton->joints.size())
{
}
