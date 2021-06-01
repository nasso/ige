#ifndef E1315A91_9C70_4B1A_B9EC_CFDC0C815201
#define E1315A91_9C70_4B1A_B9EC_CFDC0C815201

#include <concepts>
#include <cstddef>
#include <functional>
#include <glm/ext/quaternion_common.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

namespace ige::asset::animation {

enum class Interpolation {
    LINEAR,
    STEP,
};

template <typename T>
concept Animable = requires(const T& a, const T& b)
{
    T(a + 0.5f * (b - a));
};

template <typename T>
T interpolate(Interpolation alg, const T& from, const T& to, float t)
{
    switch (alg) {
    default:
    case Interpolation::LINEAR:
        return from + t * (to - from);
    case Interpolation::STEP:
        return t == 1.0f ? to : from;
    }
}

template <typename T>
glm::tquat<T> interpolate(
    Interpolation alg, const glm::tquat<T>& from, const glm::tquat<T>& to,
    float t)
{
    switch (alg) {
    default:
    case Interpolation::LINEAR:
        return glm::slerp(from, to, t);
    case Interpolation::STEP:
        return t == 1.0f ? to : from;
    }
}

template <Animable T>
class Sampler {
public:
    Sampler(
        std::map<float, T> keyframes,
        Interpolation interpolation = Interpolation::LINEAR)
        : m_keyframes(std::move(keyframes))
        , m_interpolation(interpolation)
    {
    }

    T value_at(float t) const
    {
        if (m_keyframes.empty()) {
            throw std::out_of_range("sampler doesn't have any keyframe");
        }

        auto after = m_keyframes.lower_bound(t);
        auto before = after;

        if (before != m_keyframes.begin()) {
            before--;
        }

        if (before == after) {
            // we didn't even reach the first keyframe
            // return the value of the first keyframe
            return after->second;
        } else if (after == m_keyframes.end()) {
            // we're past the last keyframe
            // return the value of the last one
            return before->second;
        } else {
            // we're between two keyframes
            // we interpolate
            const auto& [ta, a] = *before;
            const auto& [tb, b] = *after;

            return interpolate(m_interpolation, a, b, (t - ta) / (tb - ta));
        }
    }

    float duration() const
    {
        if (m_keyframes.empty()) {
            return 0.0f;
        } else {
            return (--m_keyframes.end())->first;
        }
    }

private:
    std::map<float, T> m_keyframes;
    Interpolation m_interpolation;
};

struct NodeAnimation {
    std::optional<Sampler<glm::vec3>> translation;
    std::optional<Sampler<glm::quat>> rotation;
    std::optional<Sampler<glm::vec3>> scale;

    float duration() const;
};

class AnimationClip {
private:
    using NodeMap = std::unordered_map<std::size_t, NodeAnimation>;

    NodeMap m_nodes;
    std::string m_name;

public:
    using Handle = std::shared_ptr<AnimationClip>;
    using Iterator = NodeMap::iterator;
    using ConstIterator = NodeMap::const_iterator;

    AnimationClip(std::string name = "");

    float duration() const;
    NodeAnimation& operator[](std::size_t node);

    std::string_view name() const;

    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;

    ConstIterator cbegin() const;
    ConstIterator cend() const;
};

}

namespace ige::asset {

using animation::AnimationClip;

}

#endif /* E1315A91_9C70_4B1A_B9EC_CFDC0C815201 */
