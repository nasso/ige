#include "igepch.hpp"

#include "ige/asset/AnimationClip.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/plugin/AnimationPlugin.hpp"
#include <cstddef>

using ige::asset::AnimationClip;
using ige::ecs::EntityId;
using ige::plugin::animation::AnimationTrack;
using ige::plugin::animation::Animator;

// AnimationTrack

void AnimationTrack::rewind()
{
    seek(Duration::zero());
}

void AnimationTrack::seek(Duration time)
{
    current_time = time;
}

// Animator

void Animator::set_current(std::size_t idx)
{
    for (std::size_t i = 0; i < track_count(); i++) {
        m_tracks[i].weight = 0.0f;
    }

    m_tracks[idx].weight = 1.0f;
}

void Animator::set_current(std::string_view name)
{
    auto [begin, end] = tracks(name);

    for (auto it = begin; it != end; it++) {
        set_current(it->second);
    }
}

std::size_t Animator::track_count() const
{
    return m_tracks.size();
}

bool Animator::empty() const
{
    return m_tracks.empty();
}

std::span<const AnimationTrack> Animator::tracks() const
{
    return m_tracks;
}

std::span<AnimationTrack> Animator::tracks()
{
    return m_tracks;
}

const AnimationTrack& Animator::track(std::size_t idx) const
{
    if (idx >= track_count()) {
        throw std::out_of_range("animation index out of range");
    }

    return m_tracks[idx];
}

AnimationTrack& Animator::track(std::size_t idx)
{
    if (idx >= track_count()) {
        throw std::out_of_range("animation index out of range");
    }

    return m_tracks[idx];
}

std::pair<Animator::ConstTrackIterator, Animator::ConstTrackIterator>
Animator::tracks(std::string_view name) const
{
#if __cpp_lib_generic_unordered_lookup >= 201811L
    return m_track_names.equal_range(name);
#else
    return m_track_names.equal_range(std::string(name));
#endif
}

std::pair<Animator::TrackIterator, Animator::TrackIterator>
Animator::tracks(std::string_view name)
{
#if __cpp_lib_generic_unordered_lookup >= 201811L
    return m_track_names.equal_range(name);
#else
    return m_track_names.equal_range(std::string(name));
#endif
}

std::size_t Animator::add_track(AnimationTrack track)
{
    m_tracks.push_back(std::move(track));
    return m_tracks.size() - 1;
}

void Animator::set_track_name(std::size_t track, std::string name)
{
    m_track_names.emplace(name, track);
}

// Animator::StringHash

std::size_t Animator::StringHash::operator()(const char* str) const
{
    return hash_type {}(str);
}

std::size_t Animator::StringHash::operator()(std::string_view str) const
{
    return hash_type {}(str);
}

std::size_t Animator::StringHash::operator()(const std::string& str) const
{
    return hash_type {}(str);
}
