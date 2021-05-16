#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include <cstdint>

using glm::mat4;
using glm::quat;
using glm::vec3;
using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::Transform;
using ige::plugin::TransformPlugin;

Transform Transform::make_look_at(vec3 position, vec3 target, vec3 up)
{
    Transform xform;

    xform.set_translation(position);
    xform.look_at(target, up);
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
    m_local_to_world.reset();
    m_world_to_local.reset();
}

void Transform::set_rotation(quat value)
{
    m_rotation = value;
    m_local_to_world.reset();
    m_world_to_local.reset();
}

void Transform::set_scale(vec3 value)
{
    m_scale = value;
    m_local_to_world.reset();
    m_world_to_local.reset();
}

void Transform::translate(vec3 v)
{
    m_translation += v;
    m_local_to_world.reset();
    m_world_to_local.reset();
}

void Transform::rotate(vec3 angles)
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
}

void Transform::rotate(quat rot)
{
    m_rotation *= rot;
    m_local_to_world.reset();
    m_world_to_local.reset();
}

void Transform::scale(vec3 f)
{
    m_scale *= f;
    m_local_to_world.reset();
    m_world_to_local.reset();
}

void Transform::look_at(vec3 target, vec3 up)
{
    set_rotation(glm::quatLookAt(glm::normalize(target - m_translation), up));
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
