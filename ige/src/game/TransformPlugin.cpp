#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/game/TransformPlugin.hpp"
#include <cstdint>

using glm::quat;
using glm::vec3;
using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;
using ige::game::Transform;
using ige::game::TransformPlugin;

Transform Transform::look_at(vec3 position, vec3 target, vec3 up)
{
    Transform xform;

    xform.set_translation(position);
    xform.set_rotation(glm::quatLookAt(glm::normalize(target - position), up));
    return xform;
}

vec3 Transform::translation() const
{
    return m_translation;
}

quat Transform::rotation() const
{
    return m_rotation;
}

vec3 Transform::scale() const
{
    return m_scale;
}

void Transform::set_translation(vec3 value)
{
    m_translation = value;
    m_version++;
}

void Transform::set_rotation(quat value)
{
    m_rotation = value;
    m_version++;
}

void Transform::set_scale(vec3 value)
{
    m_scale = value;
    m_version++;
}

std::uint64_t Transform::version() const
{
    return m_version;
}

static void compute_local_to_world(World&)
{
    // TODO
}

void TransformPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System(compute_local_to_world));
}
