#ifndef AA5258CB_6999_4F5C_82F1_8CBD11058CFC
#define AA5258CB_6999_4F5C_82F1_8CBD11058CFC

#include "ige/asset/AnimationClip.hpp"
#include "ige/asset/Skeleton.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/Entity.hpp"
#include <cstddef>
#include <functional>
#include <glm/mat4x4.hpp>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
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

struct AnimationTrack {
    using Duration = asset::AnimationClip::Duration;

    AnimationTrack(ecs::EntityId target, asset::AnimationClip::Handle clip);

    void rewind();
    void seek(Duration);

    ecs::EntityId target;
    asset::AnimationClip::Handle clip;
    Duration current_time = Duration::zero();
    bool loop = true;
    float weight = 0.0f;
    float playback_rate = 1.0f;
};

class Animator {
private:
    struct StringHash {
        using hash_type = std::hash<std::string_view>;
        using is_transparent = void;

        std::size_t operator()(const char*) const;
        std::size_t operator()(std::string_view) const;
        std::size_t operator()(const std::string&) const;
    };

    using TrackNameMap = std::unordered_multimap<
        std::string, std::size_t, StringHash, std::equal_to<>>;

public:
    using TrackIterator = TrackNameMap::iterator;
    using ConstTrackIterator = TrackNameMap::const_iterator;

    float playback_rate = 1.0f;

    Animator() = default;

    void set_current(std::size_t);
    void set_current(std::string_view);

    std::size_t track_count() const;
    bool empty() const;

    std::span<const AnimationTrack> tracks() const;
    std::span<AnimationTrack> tracks();

    const AnimationTrack& track(std::size_t) const;
    AnimationTrack& track(std::size_t);

    std::pair<ConstTrackIterator, ConstTrackIterator>
        tracks(std::string_view) const;
    std::pair<TrackIterator, TrackIterator> tracks(std::string_view);

    std::size_t
    add_track(ecs::EntityId target, asset::AnimationClip::Handle clip);
    void set_track_name(std::size_t track, std::string name);

private:
    bool m_paused = false;
    std::vector<AnimationTrack> m_tracks;
    TrackNameMap m_track_names;
};

class AnimationPlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* AA5258CB_6999_4F5C_82F1_8CBD11058CFC */
