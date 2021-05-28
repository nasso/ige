#include "BulletWorld.hpp"
#include "BulletRigidBody.hpp"
#include "ige/ecs.hpp"
#include "ige/plugin.hpp"

#include <algorithm>
#include <iostream>
#include <optional>

using ige::bt::BulletWorld;
using ige::ecs::EntityId;
using ige::ecs::World;
using ige::plugin::physic::PhysicWorld;
using ige::plugin::physic::RigidBody;
using ige::plugin::transform::Transform;

BulletWorld::BulletWorld(btVector3 gravity)
    : m_dispatcher(&m_collision_config)
    , m_broadphase(new btDbvtBroadphase)
    , m_world(&m_dispatcher, m_broadphase.get(), &m_solver, &m_collision_config)
{
    m_world.setGravity(gravity);
    m_world.setInternalTickCallback(tick_update, this);
}

void BulletWorld::new_entity(World& wld, EntityId entity)
{
    auto transform = wld.get_component<Transform>(entity);
    auto rigidbody = wld.get_component<RigidBody>(entity);

    wld.emplace_component<BulletRigidBody>(
        entity, *rigidbody, *transform, &m_world);
}

void BulletWorld::simulate(float time_step)
{
    m_world.stepSimulation(time_step);
}

void BulletWorld::get_collisions(World& wld, PhysicWorld& world)
{
    auto rigidbody_storage = wld.get_component_storage<BulletRigidBody>();

    std::for_each(m_collisions.begin(), m_collisions.end(), [&](auto& pair) {
        std::optional<EntityId> entity1;
        for (auto it = rigidbody_storage->begin();
             it != rigidbody_storage->end(); ++it) {
            if (it->second == pair.first)
                entity1 = it->first;
        }
        std::optional<EntityId> entity2;
        for (auto it = rigidbody_storage->begin();
             it != rigidbody_storage->end(); ++it) {
            if (it->second == pair.second)
                entity2 = it->first;
        }
        if (entity1 || entity2) {
            world.add_collision(entity1.value(), entity2.value());
        }
    });
}

void BulletWorld::tick_update(btDynamicsWorld* dynamicsWorld, btScalar)
{
    auto self = static_cast<BulletWorld*>(dynamicsWorld->getWorldUserInfo());

    self->m_collisions.clear();
    btDispatcher* const dispatcher = dynamicsWorld->getDispatcher();
    const int num_manifolds = dispatcher->getNumManifolds();
    for (int manifold_idx = 0; manifold_idx < num_manifolds; ++manifold_idx) {
        const btPersistentManifold* const manifold
            = dispatcher->getManifoldByIndexInternal(manifold_idx);

        const btRigidBody* object1
            = static_cast<const btRigidBody*>(manifold->getBody0());
        const btRigidBody* object2
            = static_cast<const btRigidBody*>(manifold->getBody1());
        self->m_collisions.push_back(
            std::pair<const btRigidBody*, const btRigidBody*> {
                object1,
                object2,
            });
    }
}
