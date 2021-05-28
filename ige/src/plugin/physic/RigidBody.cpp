#include "ige/plugin/physic/RigidBody.hpp"

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

float RigidBody::get_mass() const
{
    if (m_is_kinematic) {
        return 0;
    }
    return m_mass;
}

RigidBody& RigidBody::set_mass(float mass)
{
    m_status = RigidBodyStatus::DIRTY;
    m_mass = mass;
    return *this;
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
