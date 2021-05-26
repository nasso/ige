#include "ige/plugin/physic/RigidBody.hpp"

using ige::plugin::physic::RigidBody;
using ige::plugin::physic::RigidBodyStatus;

RigidBody::RigidBody(int mass, bool is_kinematic)
    : m_mass(mass)
    , m_is_kinematic(is_kinematic)
{
    m_status = RigidBodyStatus::NEW;
};

RigidBodyStatus RigidBody::status() const
{
    return m_status;
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
    if (m_is_kinematic)
        return 0;
    return m_mass;
};

RigidBody& RigidBody::set_mass(float mass)
{
    m_status = RigidBodyStatus::DIRTY;
    m_mass = mass;
    return *this;
}

RigidBody& RigidBody::apply_force(float force)
{
    m_status = RigidBodyStatus::DIRTY;
    m_force = force;
    return *this;
}

float RigidBody::apply_force()
{
    float old = m_force;

    m_force = 0;
    return old;
}

void RigidBody::initialize()
{
    m_status = RigidBodyStatus::CLEAN;
};

void RigidBody::update()
{
    m_status = RigidBodyStatus::CLEAN;
}
