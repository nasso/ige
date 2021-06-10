#include "ige/plugin/physics/GhostObject.hpp"

using ige::plugin::physics::Collider;
using ige::plugin::physics::GhostObject;

GhostObject::GhostObject(Collider collider)
    : m_collider(collider)
{
}

const Collider& GhostObject::collider() const
{
    return m_collider;
}
