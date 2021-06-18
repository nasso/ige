#ifndef E1315A91_9C70_4B1A_B9EC_CFDC0C815201
#define E1315A91_9C70_4B1A_B9EC_CFDC0C815201

#include <chrono>
#include <cstddef>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

namespace ige::asset::animation {

template <typename T>
struct Sampler {
    std::vector<T> samples;

    const T& operator[](std::size_t idx) const
    {
        static const T DEFAULT_VALUE {};

        if (idx >= samples.size()) {
            return samples.empty() ? DEFAULT_VALUE : samples.back();
        } else {
            return samples[idx];
        }
    }
};

struct JointSampler {
    Sampler<glm::vec3> pos_sampler;
    Sampler<glm::quat> rotation_sampler;
    Sampler<glm::vec3> scale_sampler;
};

struct AnimationClip {
    using Handle = std::shared_ptr<AnimationClip>;
    using Duration = std::chrono::steady_clock::duration;

    std::vector<JointSampler> joints;
    Duration duration = Duration::zero();
    Duration sample_duration = Duration::zero();
};

}

namespace ige::asset {

using animation::AnimationClip;

}

#endif /* E1315A91_9C70_4B1A_B9EC_CFDC0C815201 */
