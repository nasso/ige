#ifndef AA5258CB_6999_4F5C_82F1_8CBD11058CFC
#define AA5258CB_6999_4F5C_82F1_8CBD11058CFC

#include "ige/asset/AnimationClip.hpp"
#include "ige/asset/Skeleton.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/plugin/TimePlugin.hpp"
#include <cstddef>
#include <glm/mat4x4.hpp>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace ige::plugin::animation {

struct JointPose {
    glm::quat rotation;
    glm::vec3 pos;
    glm::vec3 scale;
};

struct SkeletonPose {
    asset::Skeleton::Handle skeleton;
    std::vector<JointPose> joint_pose;
    std::vector<glm::mat4> global_pose;

    SkeletonPose(asset::Skeleton::Handle);
};

struct Animation {
    ecs::EntityId target;
    asset::AnimationClip::Handle clip;
    std::string name = "";
};

struct AnimationPlayer {
    using Duration = time::Time::Duration;

    Duration current_time = Duration::zero();
    std::size_t index;
    // float factor = 1.0f;
    bool loop = true;
    bool paused = false;
};

struct Animator {
    std::vector<Animation> animations;
    // std::vector<AnimationPlayer> players;
    std::optional<AnimationPlayer> player;
};

class AnimationPlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* AA5258CB_6999_4F5C_82F1_8CBD11058CFC */
