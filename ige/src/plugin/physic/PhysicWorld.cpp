#include "ige/plugin/physic/PhysicWorld.hpp"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

using ige::plugin::physic::PhysicWorld;

PhysicWorld::PhysicWorld()
    : m_dispatcher(&m_collision_config)
    , m_broadphase(new btDbvtBroadphase)
    , m_world(&m_dispatcher, m_broadphase.get(), &m_solver, &m_collision_config)
{
    m_world.setGravity(btVector3(0, -10, 0));
}

btDiscreteDynamicsWorld* PhysicWorld::world()
{
    return &m_world;
}
