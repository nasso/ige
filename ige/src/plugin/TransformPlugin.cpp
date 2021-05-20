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
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::Children;
using ige::plugin::Parent;
using ige::plugin::Transform;
using ige::plugin::TransformPlugin;

Parent::Parent(ecs::EntityId parent_entity)
    : entity(parent_entity)
{
}

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
    m_dirty = true;
    return *this;
}

Transform& Transform::set_rotation(quat value) &
{
    m_rotation = value;
    m_dirty = true;
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

namespace systems {

namespace utils {
    static void update_transform_tree(
        World& world, EntityId entity, Transform& transform,
        const mat4& parent_xform, bool force = false)
    {
        if (transform.needs_update() || force) {
            force = true;
            transform.force_update(parent_xform);
        }

        if (auto children_ = world.get_component<Children>(entity)) {
            for (auto child : children_->get().entities) {
                auto transform_ = world.get_component<Transform>(child);

                if (transform_) {
                    update_transform_tree(
                        world, child, transform_->get(),
                        transform.local_to_world(), force);
                }
            }
        }
    }
}

static void compute_children_sets(World& world)
{
    auto all_parents = world.query<Children>();
    auto all_children = world.query<Parent>();

    // clear the children set for each parent
    for (auto [entity, children] : all_parents) {
        children.entities.clear();
    }

    // re-insert children back in their parents
    for (auto [entity, parent] : all_children) {
        auto& children
            = world.get_or_emplace_component<Children>(parent.entity);

        children.entities.insert(entity);
    }
}

static void compute_world_transform(World& world)
{
    for (auto [entity, transform] : world.query<Transform>()) {
        auto parent_ = world.get_component<Parent>(entity);

        if (parent_) {
            continue;
        }

        utils::update_transform_tree(world, entity, transform, mat4 { 1.0f });
    }
}

}

void TransformPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System(systems::compute_children_sets));
    builder.add_system(System(systems::compute_world_transform));
}
