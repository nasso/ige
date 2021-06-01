#include "ige/plugin/physic/RigidBody.hpp"
#include "glm/vec3.hpp"

using glm::vec3;
using ige::plugin::physic::Collider;
using ige::plugin::physic::RigidBody;
using ige::plugin::physic::RigidBodyStatus;

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

Collider RigidBody::collider() const
{
    return m_collider;
}

RigidBody RigidBody::set_is_kinematic(bool is_kinematic)
{
    this->is_kinematic(is_kinematic);
    return *this;
}

bool RigidBody::is_kinematic() const
{
    return m_is_kinematic;
}

void RigidBody::is_kinematic(bool is_kinematic)
{
    if (m_is_kinematic != is_kinematic) {
        m_status = RigidBodyStatus::DIRTY;
        m_is_kinematic = is_kinematic;
    }
}

RigidBody RigidBody::set_mass(float mass)
{
    this->mass(mass);
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
    if (m_mass != mass) {
        m_status = RigidBodyStatus::DIRTY;
        m_mass = mass;
    }
}

RigidBody RigidBody::set_use_gravity(bool use_gravity)
{
    this->use_gravity(use_gravity);
    return *this;
}

void RigidBody::use_gravity(bool use_gravity)
{
    if (m_use_gravity != use_gravity) {
        m_status = RigidBodyStatus::DIRTY;
        m_use_gravity = use_gravity;
    }
}

bool RigidBody::use_gravity() const
{
    return m_use_gravity;
}

void RigidBody::apply_force(const glm::vec3& force)
{
    m_status = RigidBodyStatus::DIRTY;
    m_forces.push_back(force);
}

const std::vector<glm::vec3>& RigidBody::get_forces() const
{
    return m_forces;
}

void RigidBody::clear_forces()
{
    m_forces.clear();
}

RigidBody RigidBody::set_freeze_rotation(bool freeze_rotation)
{
    this->freeze_rotation(freeze_rotation);
    return *this;
}

bool RigidBody::freeze_rotation() const
{
    return m_freeze_rotation;
}

void RigidBody::freeze_rotation(bool freeze_rotation)
{
    if (m_freeze_rotation != freeze_rotation) {
        m_status = RigidBodyStatus::DIRTY;
        m_freeze_rotation = freeze_rotation;
    }
}

RigidBody RigidBody::set_freeze_position(bool freeze_position)
{
    this->freeze_position(freeze_position);
    return *this;
}

bool RigidBody::freeze_position() const
{
    return m_freeze_position;
}

void RigidBody::freeze_position(bool freeze_position)
{
    if (m_freeze_position != freeze_position) {
        m_status = RigidBodyStatus::DIRTY;
        m_freeze_position = freeze_position;
    }
}

RigidBody RigidBody::set_center_of_mass(const glm::vec3& center_of_mass)
{
    this->center_of_mass(center_of_mass);
    return *this;
}

vec3 RigidBody::center_of_mass() const
{
    return m_center_of_mass;
}

void RigidBody::center_of_mass(const glm::vec3& center_of_mass)
{
    if (m_center_of_mass != center_of_mass) {
        m_status = RigidBodyStatus::DIRTY;
        m_center_of_mass = center_of_mass;
    }
}

void RigidBody::reset_center_of_mass()
{
    if (m_center_of_mass != vec3 { 0.f }) {
        m_status = RigidBodyStatus::DIRTY;
        m_center_of_mass = vec3 { 0.f };
    }
}

RigidBody RigidBody::set_velocity(const glm::vec3& velocity)
{
    this->velocity(velocity);
    return *this;
}

void RigidBody::velocity(const glm::vec3& velocity)
{
    if (m_velocity != velocity || velocity != vec3 { 0.f }) {
        m_status = RigidBodyStatus::DIRTY;
        m_velocity = velocity;
    }
}

glm::vec3 RigidBody::velocity() const
{
    return m_velocity;
}
