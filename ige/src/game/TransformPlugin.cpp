#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/game/TransformPlugin.hpp"
#include <cstdint>

using glm::mat4;
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

const mat4& Transform::compute_matrix()
{
    if (!m_local_to_world) {
        mat4 mat(1.0f);

        mat = glm::translate(mat, m_translation) * glm::mat4_cast(m_rotation);
        m_local_to_world = glm::scale(mat, m_scale);
    }

    return *m_local_to_world;
}

const mat4& Transform::compute_inverse()
{
    if (!m_world_to_local) {
        m_world_to_local = glm::inverse(compute_matrix());
    }

    return *m_world_to_local;
}

void TransformPlugin::plug(App::Builder&) const
{
}
