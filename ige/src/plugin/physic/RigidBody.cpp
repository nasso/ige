#include "ige/plugin/physic/RigidBody.hpp"

#include "glm/vec3.hpp"
; // TODO: https://bit.ly/3hhMJ58

using glm::vec3;
using ige::plugin::physic::Collider;
using ige::plugin::physic::RigidBody;
using ige::plugin::physic::RigidBodyStatus;

RigidBody::RigidBody(Collider collider, int mass, bool is_kinematic)
    : m_collider(collider)
    , m_mass(mass)
    , m_is_kinematic(is_kinematic)
{
    m_status = RigidBodyStatus::CLEAN;
}

RigidBodyStatus RigidBody::status() const
{
    return m_status;
}

Collider RigidBody::collider() const
{
    return m_collider;
}

bool RigidBody::is_kinematic() const
{
    return m_is_kinematic;
}

RigidBody& RigidBody::is_kinematic(bool is_kinematic)
{
    m_status = RigidBodyStatus::DIRTY;
    m_is_kinematic = is_kinematic;
    return *this;
}

float RigidBody::mass() const
{
    if (m_is_kinematic) {
        return 0;
    }
    return m_mass;
}

void RigidBody::mass(float mass)
{
    m_status = RigidBodyStatus::DIRTY;
    m_mass = mass;
}

void RigidBody::use_gravity(bool use_gravity)
{
    m_status = RigidBodyStatus::DIRTY;
    m_use_gravity = use_gravity;
}

bool RigidBody::use_gravity() const
{
    return m_use_gravity;
}

RigidBody& RigidBody::apply_force(glm::vec3 force)
{
    m_status = RigidBodyStatus::DIRTY;
    m_forces.push_back(force);
    return *this;
}

const std::vector<glm::vec3>& RigidBody::get_forces() const
{
    return m_forces;
}

void RigidBody::clear_forces()
{
    m_forces.clear();
}

void RigidBody::update()
{
    m_status = RigidBodyStatus::CLEAN;
}

bool RigidBody::freeze_rotation() const
{
    return m_freeze_rotation;
}

void RigidBody::freeze_rotation(bool freeze_rotation)
{
    m_status = RigidBodyStatus::DIRTY;
    m_freeze_rotation = freeze_rotation;
}

bool RigidBody::freeze_position() const
{
    return m_freeze_position;
}

void RigidBody::freeze_position(bool freeze_position)
{
    m_status = RigidBodyStatus::DIRTY;
    m_freeze_position = freeze_position;
}

vec3 RigidBody::center_of_mass() const
{
    return m_center_of_mass;
}

void RigidBody::center_of_mass(glm::vec3 center_of_mass)
{
    m_status = RigidBodyStatus::DIRTY;
    m_center_of_mass = center_of_mass;
}

void RigidBody::reset_center_of_mass()
{
    m_status = RigidBodyStatus::DIRTY;
    m_center_of_mass = vec3 { 0.f };
}
