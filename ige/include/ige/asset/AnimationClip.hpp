#ifndef E1315A91_9C70_4B1A_B9EC_CFDC0C815201
#define E1315A91_9C70_4B1A_B9EC_CFDC0C815201

#include <chrono>
#include <cstddef>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <string>
#include <vector>

namespace ige::asset::animation {

template <typename T>
struct Sampler {
    std::vector<T> samples;

    const T& operator[](std::size_t idx) const
    {
        if (idx >= samples.size()) {
            return samples.empty() ? T {} : samples.back();
        } else {
            return samples[idx];
        }
    }
};

struct JointSampler {
    Sampler<glm::vec4> pos_scale_samples;
    Sampler<glm::quat> rotation_samples;
};

struct AnimationClip {
    using Handle = std::shared_ptr<AnimationClip>;

    std::string name = "";
    float duration = 0.0f;
    std::vector<JointSampler> joints;
    std::chrono::steady_clock::duration sample_duration;
};

}

namespace ige::asset {

using animation::AnimationClip;

}

#endif /* E1315A91_9C70_4B1A_B9EC_CFDC0C815201 */
