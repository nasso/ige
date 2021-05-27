#ifndef AA5258CB_6999_4F5C_82F1_8CBD11058CFC
#define AA5258CB_6999_4F5C_82F1_8CBD11058CFC

#include "ige/asset/AnimationClip.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/plugin/TimePlugin.hpp"
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace ige::plugin::animation {

struct JointPose {
    glm::quat rotation;
    glm::vec3 pos;
    glm::vec3 scale;
};

struct SkeletonPose {
    std::vector<JointPose> joint_poses;
};

struct Animator {
    using Duration = time::Time::Duration;

    bool loop = true;
    bool paused = false;
    Duration current_time = Duration::zero();
    asset::AnimationClip::Handle current_animation = nullptr;
};

class AnimationPlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* AA5258CB_6999_4F5C_82F1_8CBD11058CFC */
