#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include <cstdint>
#include <utility>

using glm::mat4;
using glm::quat;
using glm::vec3;
using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::Transform;
using ige::plugin::TransformPlugin;

Transform Transform::from_pos(vec3 position)
{
    Transform xform;

    xform.set_translation(position);
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

Transform& Transform::set_translation(vec3 value) &
{
    m_translation = value;
    m_local_to_world.reset();
    m_world_to_local.reset();
    return *this;
}

Transform& Transform::set_rotation(quat value) &
{
    m_rotation = value;
    m_local_to_world.reset();
    m_world_to_local.reset();
    return *this;
}

Transform& Transform::set_scale(vec3 value) &
{
    m_scale = value;
    m_local_to_world.reset();
    m_world_to_local.reset();
    return *this;
}

Transform& Transform::translate(vec3 v)
{
    m_translation += v;
    m_local_to_world.reset();
    m_world_to_local.reset();
    return *this;
}

Transform& Transform::rotate(vec3 angles)
{
    const vec3 X_AXIS { 1.0f, 0.0f, 0.0f };
    const vec3 Y_AXIS { 0.0f, 1.0f, 0.0f };
    const vec3 Z_AXIS { 0.0f, 0.0f, 1.0f };

    // order of rotation: ZXY (same as Unity)
    m_rotation = glm::rotate(m_rotation, glm::radians(angles.z), Z_AXIS);
    m_rotation = glm::rotate(m_rotation, glm::radians(angles.x), X_AXIS);
    m_rotation = glm::rotate(m_rotation, glm::radians(angles.y), Y_AXIS);
    m_local_to_world.reset();
    m_world_to_local.reset();
    return *this;
}

Transform& Transform::rotate(quat rot)
{
    m_rotation *= rot;
    m_local_to_world.reset();
    m_world_to_local.reset();
    return *this;
}

Transform& Transform::scale(vec3 f)
{
    m_scale *= f;
    m_local_to_world.reset();
    m_world_to_local.reset();
    return *this;
}

Transform& Transform::look_at(vec3 target, vec3 up) &
{
    set_rotation(glm::quatLookAt(glm::normalize(target - m_translation), up));
    return *this;
}

Transform Transform::set_translation(vec3 value) &&
{
    return std::move(set_translation(value));
}

Transform Transform::set_rotation(quat value) &&
{
    return std::move(set_rotation(value));
}

Transform Transform::set_scale(vec3 value) &&
{
    return std::move(set_scale(value));
}

Transform Transform::look_at(vec3 target, vec3 up) &&
{
    return std::move(look_at(target, up));
}

mat4 Transform::compute_matrix()
{
    if (!m_local_to_world) {
        mat4 mat(1.0f);

        mat = glm::translate(mat, m_translation) * glm::mat4_cast(m_rotation);
        m_local_to_world = glm::scale(mat, m_scale);
    }

    return *m_local_to_world;
}

mat4 Transform::compute_inverse()
{
    if (!m_world_to_local) {
        m_world_to_local = glm::inverse(compute_matrix());
    }

    return *m_world_to_local;
}

void TransformPlugin::plug(App::Builder&) const
{
}
