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

const std::vector<Collision>& PhysicsWorld::collisions() const
{
    return m_collisions;
}

bool PhysicsWorld::collide(
    const ecs::EntityId& entity1, const ecs::EntityId& entity2) const
{
    for (const auto& [fst_entity, snd_entity] : m_collisions) {
        if (fst_entity == entity1 && snd_entity == entity2) {
            return true;
        }

        if (fst_entity == entity2 && snd_entity == entity1) {
            return true;
        }
    }

    return false;
}

void PhysicsWorld::clear_collisions()
{
    m_collisions.clear();
}

void PhysicsWorld::add_constraint(const Constraint& constraint)
{
    m_new_constraints.push_back(constraint);
}

const std::vector<Constraint>& PhysicsWorld::new_constraints() const
{
    return m_new_constraints;
}

void PhysicsWorld::clear_new_constraints()
{
    m_new_constraints.clear();
}
