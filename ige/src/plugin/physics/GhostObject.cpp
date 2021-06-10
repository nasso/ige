#include "ige/plugin/physics/GhostObject.hpp"
#include "glm/vec3.hpp"
#include <utility>

using glm::vec3;
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
