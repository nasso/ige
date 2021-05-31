#include "ige/plugin/physic/PhysicWorld.hpp"
#include "ige/ecs/Entity.hpp"

using ige::ecs::EntityId;
using ige::plugin::physic::Collision;
using ige::plugin::physic::Constraint;
using ige::plugin::physic::PhysicWorld;

void PhysicWorld::add_collision(ecs::EntityId entity1, ecs::EntityId entity2)
{
    m_collisions.emplace_back(entity1, entity2);
}

const std::vector<Collision>& PhysicWorld::get_collisions() const
{
    return m_collisions;
}

bool PhysicWorld::collide(ecs::EntityId entity1, ecs::EntityId entity2)
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

void PhysicWorld::clear_collisions()
{
    m_collisions.clear();
}

void PhysicWorld::add_constraint(const Constraint& constraint)
{
    m_new_constraints.push_back(constraint);
}

const std::vector<Constraint>& PhysicWorld::get_new_constraints() const
{
    return m_new_constraints;
}

void PhysicWorld::clear_new_constraints()
{
    m_new_constraints.clear();
}
