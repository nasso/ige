#include "ige/plugin/TransformPlugin.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <utility>

using glm::mat4;
using glm::quat;
using glm::vec3;
using glm::vec4;
using ige::plugin::transform::Parent;
using ige::plugin::transform::Transform;

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

vec3 Transform::world_translation() const
{
    return local_to_world() * vec4(0.0f, 0.0f, 0.0f, 1.0f);
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
    m_dirty = true;
    return *this;
}

Transform& Transform::set_rotation(quat value) &
{
    m_rotation = value;
    m_dirty = true;
    return *this;
}

Transform& Transform::set_rotation(vec3 angles) &
{
    m_rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
    rotate(angles);
    // rotate() already set the dirty flag
    // m_dirty = true;
    return *this;
}

Transform& Transform::set_scale(vec3 value) &
{
    m_scale = value;
    m_dirty = true;
    return *this;
}

Transform& Transform::set_scale(float value) &
{
    return set_scale(vec3 { value });
}

Transform& Transform::translate(vec3 v)
{
    m_translation += v;
    m_dirty = true;
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
    m_dirty = true;
    return *this;
}

Transform& Transform::rotate(quat rot)
{
    m_rotation *= rot;
    m_dirty = true;
    return *this;
}

Transform& Transform::scale(vec3 f)
{
    m_scale *= f;
    m_dirty = true;
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

Transform Transform::set_rotation(vec3 angles) &&
{
    return std::move(set_rotation(angles));
}

Transform Transform::set_scale(vec3 value) &&
{
    return std::move(set_scale(value));
}

Transform Transform::set_scale(float value) &&
{
    return std::move(set_scale(value));
}

Transform Transform::look_at(vec3 target, vec3 up) &&
{
    return std::move(look_at(target, up));
}

void Transform::force_update(const mat4& parent)
{
    mat4 mat = parent;

    mat = glm::translate(mat, m_translation) * glm::mat4_cast(m_rotation);
    m_local_to_world = glm::scale(mat, m_scale);
    m_world_to_local = glm::inverse(m_local_to_world);

    m_dirty = false;
}

bool Transform::needs_update() const
{
    return m_dirty;
}

const mat4& Transform::local_to_world() const
{
    return m_local_to_world;
}

const mat4& Transform::world_to_local() const
{
    return m_world_to_local;
}
