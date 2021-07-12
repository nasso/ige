#include "igepch.hpp"

#include "ige/plugin/physics/RigidBody.hpp"

using glm::vec3;
using ige::plugin::physics::Collider;
using ige::plugin::physics::RigidBody;
using ige::plugin::physics::RigidBodyStatus;

RigidBody::RigidBody(Collider collider, float mass, bool is_kinematic)
    : m_collider(collider)
    , m_mass(mass)
    , m_is_kinematic(is_kinematic)
{
    m_status = RigidBodyStatus::CLEAN;
}

bool RigidBody::is_dirty() const
{
    return m_status == RigidBodyStatus::DIRTY;
}

void RigidBody::clean()
{
    m_status = RigidBodyStatus::CLEAN;
}

const Collider& RigidBody::collider() const
{
    return m_collider;
}

bool RigidBody::is_kinematic() const
{
    return m_is_kinematic;
}

float RigidBody::mass() const
{
    if (m_is_kinematic) {
        return 0;
    }
    return m_mass;
}

bool RigidBody::use_gravity() const
{
    return m_use_gravity;
}

void RigidBody::apply_force(const glm::vec3& force)
{
    m_status = RigidBodyStatus::DIRTY;
    m_forces += force;
}

const glm::vec3& RigidBody::get_forces() const
{
    return m_forces;
}

void RigidBody::clear_forces()
{
    m_forces = { 0.0f, 0.0f, 0.0f };
}

bool RigidBody::freeze_rotation() const
{
    return m_freeze_rotation;
}

bool RigidBody::freeze_position() const
{
    return m_freeze_position;
}

const vec3& RigidBody::center_of_mass() const
{
    return m_center_of_mass;
}

const glm::vec3& RigidBody::velocity() const
{
    return m_velocity;
}

void RigidBody::reset_center_of_mass()
{
    if (m_center_of_mass != vec3 { 0.0f }) {
        m_status = RigidBodyStatus::DIRTY;
        m_center_of_mass = vec3 { 0.0f };
    }
}

RigidBody& RigidBody::set_kinematic(bool is_kinematic) &
{
    if (m_is_kinematic != is_kinematic) {
        m_status = RigidBodyStatus::DIRTY;
        m_is_kinematic = is_kinematic;
    }
    return *this;
}

RigidBody& RigidBody::set_mass(float mass) &
{
    if (m_mass != mass) {
        m_status = RigidBodyStatus::DIRTY;
        m_mass = mass;
    }
    return *this;
}
RigidBody& RigidBody::set_use_gravity(bool use_gravity) &
{
    if (m_use_gravity != use_gravity) {
        m_status = RigidBodyStatus::DIRTY;
        m_use_gravity = use_gravity;
    }
    return *this;
}
RigidBody& RigidBody::set_freeze_rotation(bool freeze_rotation) &
{
    if (m_freeze_rotation != freeze_rotation) {
        m_status = RigidBodyStatus::DIRTY;
        m_freeze_rotation = freeze_rotation;
    }
    return *this;
}

RigidBody& RigidBody::set_freeze_position(bool freeze_position) &
{
    if (m_freeze_position != freeze_position) {
        m_status = RigidBodyStatus::DIRTY;
        m_freeze_position = freeze_position;
    }
    return *this;
}

RigidBody& RigidBody::set_center_of_mass(const glm::vec3& center_of_mass) &
{
    if (m_center_of_mass != center_of_mass) {
        m_status = RigidBodyStatus::DIRTY;
        m_center_of_mass = center_of_mass;
    }
    return *this;
}

RigidBody& RigidBody::set_velocity(const glm::vec3& velocity) &
{
    if (m_velocity != velocity || velocity != vec3 { 0.0f }) {
        m_status = RigidBodyStatus::DIRTY;
        m_velocity = velocity;
    }
    return *this;
}

RigidBody RigidBody::set_kinematic(bool is_kinematic) &&
{
    return std::move(set_kinematic(is_kinematic));
}

RigidBody RigidBody::set_mass(float mass) &&
{
    return std::move(set_mass(mass));
}

RigidBody RigidBody::set_use_gravity(bool use_gravity) &&
{
    return std::move(set_use_gravity(use_gravity));
}

RigidBody RigidBody::set_velocity(const glm::vec3& velocity) &&
{
    return std::move(set_velocity(velocity));
}

RigidBody RigidBody::set_freeze_rotation(bool freeze_rotation) &&
{
    return std::move(set_freeze_rotation(freeze_rotation));
}

RigidBody RigidBody::set_freeze_position(bool freeze_position) &&
{
    return std::move(set_freeze_position(freeze_position));
}

RigidBody RigidBody::set_center_of_mass(const glm::vec3& center_of_mass) &&
{
    return std::move(set_center_of_mass(center_of_mass));
}
