#ifndef A17BC820_E47A_445F_A85E_1D87FE8FB1EA
#define A17BC820_E47A_445F_A85E_1D87FE8FB1EA

#include "ige/ecs.hpp"
#include "ige/plugin/physic/PhysicWorld.hpp"

#include "btBulletDynamicsCommon.h"
#include <memory>
#include <utility>
#include <vector>

namespace ige::bt {
class BulletWorld {
public:
    BulletWorld(btVector3 gravity = { 0, -10, 0 });

    void new_entity(ecs::World& wld, ecs::EntityId entity);

    void simulate(float time_step);

    void get_collisions(ecs::World& wld, plugin::physic::PhysicWorld& world);

    static void tick_update(btDynamicsWorld* world, btScalar timeStep);

private:
    btDefaultCollisionConfiguration m_collision_config;
    btCollisionDispatcher m_dispatcher;
    std::unique_ptr<btBroadphaseInterface> m_broadphase;
    btSequentialImpulseConstraintSolver m_solver;
    btDiscreteDynamicsWorld m_world;
    std::vector<std::pair<const btRigidBody*, const btRigidBody*>> m_collisions;
};
}

#endif /* A17BC820_E47A_445F_A85E_1D87FE8FB1EA */
