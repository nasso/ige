#include "ige/plugin/physics/PhysicsWorld.hpp"
#include "ige/ecs/Entity.hpp"

using ige::ecs::EntityId;
using ige::plugin::physics::Collision;
using ige::plugin::physics::Constraint;
using ige::plugin::physics::PhysicsWorld;

void PhysicsWorld::add_collision(
    const ecs::EntityId& entity1, const ecs::EntityId& entity2)
{
    m_collisions.emplace_back(entity1, entity2);
}

const std::vector<Collision>& PhysicsWorld::get_collisions() const
{
    return m_collisions;
}

bool PhysicsWorld::collide(
    const ecs::EntityId& entity1, const ecs::EntityId& entity2) const
{
    return std::find_if(
               m_collisions.begin(), m_collisions.end(),
               [entity1, entity2](auto& collision) {
                   if (collision.first == entity1
                       && collision.second == entity2) {
                       return true;
                   } else if (
                       collision.first == entity2
                       && collision.second == entity1) {
                       return true;
                   } else {
                       return false;
                   }
               })
        != m_collisions.end();
}

void PhysicsWorld::clear_collisions()
{
    m_collisions.clear();
}

void PhysicsWorld::add_constraint(const Constraint& constraint)
{
    m_new_constraints.push_back(constraint);
}

const std::vector<Constraint>& PhysicsWorld::get_new_constraints() const
{
    return m_new_constraints;
}

void PhysicsWorld::clear_new_constraints()
{
    m_new_constraints.clear();
}
